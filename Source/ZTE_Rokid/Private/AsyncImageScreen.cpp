#include "AsyncImageScreen.h"
#include "ZTERokidRuntimeConfig.h"
#include "Async/Async.h"
#include "Rendering/Texture2DResource.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Common/TcpSocketBuilder.h"
#include "Kismet/GameplayStatics.h"

AAsyncImageScreen::AAsyncImageScreen()
{
    PrimaryActorTick.bCanEverTick = true;
    ScreenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScreenMesh"));
    RootComponent = ScreenMesh;

    bIsRunning = false;
    bClientConnected = false;
    bIsFirstData = false;
    TextureRegion = nullptr;
    ListenerSocket = nullptr;
    ClientSocket = nullptr;
}

void AAsyncImageScreen::BeginPlay()
{
    Super::BeginPlay();

    if (BaseMaterial)
    {
        DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        ScreenMesh->SetMaterial(0, DynamicMaterial);
    }

    ScreenMesh->SetVisibility(false);

    if (UWorld* World = GetWorld())
    {
        TArray<AActor*> AllActors;
        UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), AllActors);

        UE_LOG(LogTemp, Log, TEXT("[SceneActors] Count=%d"), AllActors.Num());
        for (AActor* Actor : AllActors)
        {
            if (!Actor)
            {
                continue;
            }

            UE_LOG(LogTemp, Log, TEXT("[SceneActors] Name=%s Class=%s Location=%s"),
                *Actor->GetName(),
                *Actor->GetClass()->GetName(),
                *Actor->GetActorLocation().ToString());
        }
    }

    StartTCPServer();
}

void AAsyncImageScreen::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    bIsRunning = false;
    bClientConnected = false;

    {
        FScopeLock Lock(&SocketGuard);

        if (ClientSocket)
        {
            ClientSocket->Close();
        }

        if (ListenerSocket)
        {
            ListenerSocket->Close();
        }
    }

    if (TextureRegion)
    {
        delete TextureRegion;
        TextureRegion = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

bool AAsyncImageScreen::LoadIPConfig(FString& OutIP, int32& OutPort)
{
    return FZTERokidRuntimeConfig::LoadEndpoint(TEXT("video_receiver"), OutIP, OutPort);
}

void AAsyncImageScreen::StartTCPServer()
{
    bIsRunning = true;

    Async(EAsyncExecution::Thread, [this]()
        {
            Thread_ServerLoop();
        });
}

void AAsyncImageScreen::Thread_ServerLoop()
{
    FString IP;
    int32 Port = 0;
    if (LoadIPConfig(IP, Port))
    {
        ListenPort = Port;
    }

    {
        FScopeLock Lock(&SocketGuard);
        ListenerSocket = FTcpSocketBuilder(TEXT("RGBServerListener"))
            .AsReusable()
            .BoundToPort(ListenPort)
            .Listening(8)
            .Build();
    }

    if (!ListenerSocket)
    {
        return;
    }

    while (bIsRunning)
    {
        FSocket* LocalListener = nullptr;
        {
            FScopeLock Lock(&SocketGuard);
            LocalListener = ListenerSocket;
        }

        if (!LocalListener)
        {
            break;
        }

        bool bHasPendingConnection = false;
        if (LocalListener->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(0.5)))
        {
            LocalListener->HasPendingConnection(bHasPendingConnection);
        }

        if (!bHasPendingConnection)
        {
            continue;
        }

        TSharedRef<FInternetAddr> RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
        FString ClientDesc = TEXT("Unknown");
        FSocket* TempClientSocket = LocalListener->Accept(*RemoteAddr, ClientDesc);

        if (!TempClientSocket)
        {
            continue;
        }

        {
            FScopeLock Lock(&SocketGuard);
            if (!bIsRunning)
            {
                TempClientSocket->Close();
                ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(TempClientSocket);
                return;
            }
            ClientSocket = TempClientSocket;
        }

        int32 NewSize = 0;
        TempClientSocket->SetReceiveBufferSize(4 * 1024 * 1024, NewSize);
        bClientConnected = true;

        while (bIsRunning && bClientConnected)
        {
            FSocket* LocalClient = nullptr;
            {
                FScopeLock Lock(&SocketGuard);
                LocalClient = ClientSocket;
            }

            if (!LocalClient)
            {
                break;
            }

            if (!LocalClient->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(0.1)))
            {
                continue;
            }

            uint8 HeaderBuffer[4] = { 0 };
            int32 BytesRead = 0;
            int32 BytesReadTotal = 0;

            while (BytesReadTotal < 4 && bIsRunning && bClientConnected)
            {
                if (!LocalClient)
                {
                    break;
                }

                if (LocalClient->Recv(HeaderBuffer + BytesReadTotal, 4 - BytesReadTotal, BytesRead))
                {
                    if (BytesRead > 0)
                    {
                        BytesReadTotal += BytesRead;
                    }
                    else
                    {
                        bClientConnected = false;
                    }
                }
                else
                {
                    bClientConnected = false;
                }
            }

            if (!bClientConnected || !bIsRunning)
            {
                break;
            }

            uint32 FrameSize = 0;
            FMemory::Memcpy(&FrameSize, HeaderBuffer, 4);

            if (FrameSize == 0 || FrameSize > 100 * 1024 * 1024)
            {
                bClientConnected = false;
                break;
            }

            TArray<uint8> FrameBuffer;
            FrameBuffer.SetNumUninitialized(FrameSize);
            BytesReadTotal = 0;

            while (BytesReadTotal < (int32)FrameSize && bIsRunning && bClientConnected)
            {
                if (!LocalClient)
                {
                    break;
                }

                if (!LocalClient->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(2.0)))
                {
                    bClientConnected = false;
                    break;
                }

                if (LocalClient->Recv(FrameBuffer.GetData() + BytesReadTotal, FrameSize - BytesReadTotal, BytesRead))
                {
                    if (BytesRead > 0)
                    {
                        BytesReadTotal += BytesRead;
                    }
                    else
                    {
                        bClientConnected = false;
                    }
                }
                else
                {
                    bClientConnected = false;
                }
            }

            if (!bClientConnected || !bIsRunning)
            {
                break;
            }

            {
                FScopeLock Lock(&DataGuard);
                if (RgbBufferQueue.Num() >= 5)
                {
                    RgbBufferQueue.RemoveAt(0);
                }
                RgbBufferQueue.Add(MoveTemp(FrameBuffer));
            }
        }

        {
            FScopeLock Lock(&SocketGuard);
            if (ClientSocket)
            {
                ClientSocket->Close();
                ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
                ClientSocket = nullptr;
            }
        }
        bClientConnected = false;

        AsyncTask(ENamedThreads::GameThread, [WeakThis = TWeakObjectPtr<AAsyncImageScreen>(this)]()
            {
                if (WeakThis.IsValid())
                {
                    WeakThis->ResetScreenForNextConnection();
                }
            });
    }

    {
        FScopeLock Lock(&SocketGuard);

        if (ClientSocket)
        {
            ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
            ClientSocket = nullptr;
        }

        if (ListenerSocket)
        {
            ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenerSocket);
            ListenerSocket = nullptr;
        }
    }
}

void AAsyncImageScreen::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    TArray<uint8> CompressedData;
    bool bHasData = false;

    {
        FScopeLock Lock(&DataGuard);
        if (RgbBufferQueue.Num() > 0)
        {
            CompressedData = MoveTemp(RgbBufferQueue[0]);
            RgbBufferQueue.RemoveAt(0);
            bHasData = true;
        }
    }

    if (bHasData)
    {
        ProcessRGBFrame(CompressedData);
    }
}

void AAsyncImageScreen::ProcessRGBFrame(const TArray<uint8>& CompressedData)
{
    if (!IsValid(this))
    {
        return;
    }

    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    EImageFormat Format = ImageWrapperModule.DetectImageFormat(CompressedData.GetData(), CompressedData.Num());
    if (Format == EImageFormat::Invalid)
    {
        return;
    }

    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(Format);
    if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(CompressedData.GetData(), CompressedData.Num()))
    {
        return;
    }

    TArray<uint8> RawData;
    if (!ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, RawData))
    {
        return;
    }

    const int32 Width = ImageWrapper->GetWidth();
    const int32 Height = ImageWrapper->GetHeight();

    if (!bIsFirstData)
    {
        bIsFirstData = true;
        ScreenMesh->SetVisibility(true);
    }

    if (!TargetTexture || !TargetTexture->IsValidLowLevel() || TargetTexture->GetSizeX() != Width || TargetTexture->GetSizeY() != Height)
    {
        TargetTexture = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);
        TargetTexture->SRGB = true;
        TargetTexture->NeverStream = true;
        TargetTexture->UpdateResource();

        if (DynamicMaterial)
        {
            DynamicMaterial->SetTextureParameterValue(FName("MainTexture"), TargetTexture);
        }

        if (TextureRegion)
        {
            delete TextureRegion;
        }
        TextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, Width, Height);
    }

    if (!TargetTexture || !TargetTexture->GetResource())
    {
        return;
    }

    uint8* MemData = static_cast<uint8*>(FMemory::Malloc(RawData.Num()));
    FMemory::Memcpy(MemData, RawData.GetData(), RawData.Num());
    UpdateTextureRegions(TargetTexture, 0, 1, TextureRegion, Width * 4, 4, MemData, true);
}

void AAsyncImageScreen::ResetScreenForNextConnection()
{
    {
        FScopeLock Lock(&DataGuard);
        RgbBufferQueue.Empty();
    }

    bIsFirstData = false;

    if (ScreenMesh)
    {
        ScreenMesh->SetVisibility(false);
    }

    if (DynamicMaterial)
    {
        DynamicMaterial->SetTextureParameterValue(FName("MainTexture"), nullptr);
    }

    TargetTexture = nullptr;

    if (TextureRegion)
    {
        delete TextureRegion;
        TextureRegion = nullptr;
    }
}

void AAsyncImageScreen::UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData)
{
    if (Texture && Texture->GetResource())
    {
        struct FUpdateTextureRegionsData
        {
            FTexture2DResource* Texture2DResource;
            int32 MipIndex;
            uint32 NumRegions;
            FUpdateTextureRegion2D* Regions;
            uint32 SrcPitch;
            uint32 SrcBpp;
            uint8* SrcData;
        };

        FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;
        RegionData->Texture2DResource = (FTexture2DResource*)Texture->GetResource();
        RegionData->MipIndex = MipIndex;
        RegionData->NumRegions = NumRegions;
        RegionData->Regions = Regions;
        RegionData->SrcPitch = SrcPitch;
        RegionData->SrcBpp = SrcBpp;
        RegionData->SrcData = SrcData;

        ENQUEUE_RENDER_COMMAND(UpdateTextureRegionsData)(
            [RegionData, bFreeData](FRHICommandListImmediate& RHICmdList)
            {
                for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
                {
                    if (!RegionData->Texture2DResource)
                    {
                        continue;
                    }

                    const int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
                    if (RegionData->MipIndex < CurrentFirstMip)
                    {
                        continue;
                    }

                    RHIUpdateTexture2D(
                        RegionData->Texture2DResource->GetTexture2DRHI(),
                        RegionData->MipIndex - CurrentFirstMip,
                        RegionData->Regions[RegionIndex],
                        RegionData->SrcPitch,
                        RegionData->SrcData
                        + RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
                        + RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
                    );
                }

                if (bFreeData)
                {
                    FMemory::Free(RegionData->SrcData);
                }
                delete RegionData;
            });
    }
    else if (bFreeData)
    {
        FMemory::Free(SrcData);
    }
}
