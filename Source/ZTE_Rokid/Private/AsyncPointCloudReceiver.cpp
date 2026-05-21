#include "AsyncPointCloudReceiver.h"
#include "ZTERokidRuntimeConfig.h"

#include "Async/Async.h"
#include "Common/TcpSocketBuilder.h"

AAsyncPointCloudReceiver::AAsyncPointCloudReceiver()
{
    PrimaryActorTick.bCanEverTick = true;

    bIsRunning = false;
    bClientConnected = false;

    ListenerSocket = nullptr;
    ClientSocket = nullptr;
}

void AAsyncPointCloudReceiver::BeginPlay()
{
    Super::BeginPlay();

    StartTCPServer();
}

void AAsyncPointCloudReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

    Super::EndPlay(EndPlayReason);
}

void AAsyncPointCloudReceiver::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GetWorld() && GetWorld()->GetTimeSeconds() >= NextUpdateTime)
    {
        const int32 QueueSize = GetQueueSize();
        const float Fps = FrameCount / FpsInterval;

        if (FrameCount > 0)
        {
            UE_LOG(LogTemp, Log, TEXT("[PCL Receiver] FPS: %.1f (Queue: %d)"), Fps, QueueSize);
        }

        FrameCount = 0;
        NextUpdateTime = GetWorld()->GetTimeSeconds() + FpsInterval;
    }
}

bool AAsyncPointCloudReceiver::LoadIPConfig(FString& OutIP, int32& OutPort)
{
    return FZTERokidRuntimeConfig::LoadEndpoint(TEXT("pcl_receiver"), OutIP, OutPort);
}

bool AAsyncPointCloudReceiver::PopPointCloudFrame(TArray<uint8>& OutFrame)
{
    FScopeLock Lock(&DataGuard);
    if (PointCloudQueue.Num() <= 0)
    {
        return false;
    }

    OutFrame = MoveTemp(PointCloudQueue[0]);
    PointCloudQueue.RemoveAt(0);
    return true;
}

int32 AAsyncPointCloudReceiver::GetQueueSize() const
{
    FScopeLock Lock(&DataGuard);
    return PointCloudQueue.Num();
}

void AAsyncPointCloudReceiver::StartTCPServer()
{
    bIsRunning = true;

    Async(EAsyncExecution::Thread, [this]()
        {
            Thread_ServerLoop();
        });
}

void AAsyncPointCloudReceiver::EnqueueFrame(const TArray<uint8>& FrameBytes)
{
    FScopeLock Lock(&DataGuard);

    if (PointCloudQueue.Num() >= MaxQueueSize)
    {
        PointCloudQueue.RemoveAt(0);
    }

    PointCloudQueue.Add(FrameBytes);
}

void AAsyncPointCloudReceiver::Thread_ServerLoop()
{
    FString IP;
    int32 Port = 0;
    if (LoadIPConfig(IP, Port))
    {
        ListenPort = Port;
        UE_LOG(LogTemp, Log, TEXT("Create Listener Socket on port %d"), ListenPort);
    }

    {
        FScopeLock Lock(&SocketGuard);

        ListenerSocket = FTcpSocketBuilder(TEXT("PCLServerListener"))
            .AsReusable()
            .BoundToPort(ListenPort)
            .Listening(8)
            .Build();
    }

    if (!ListenerSocket)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create PCL Listener Socket on port %d"), ListenPort);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("PCL Server Started! Listening on port %d..."), ListenPort);

    while (bIsRunning)
    {
        FSocket* LocalListener = nullptr;
        {
            FScopeLock Lock(&SocketGuard);
            LocalListener = ListenerSocket;
        }

        if (!LocalListener) break;

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

        UE_LOG(LogTemp, Warning, TEXT(">>> PCL Client Connected: %s <<<"), *RemoteAddr->ToString(true));

        int32 NewSize = 0;
        TempClientSocket->SetReceiveBufferSize(8 * 1024 * 1024, NewSize);

        bClientConnected = true;

        while (bIsRunning && bClientConnected)
        {
            FSocket* LocalClient = nullptr;
            {
                FScopeLock Lock(&SocketGuard);
                LocalClient = ClientSocket;
            }

            if (!LocalClient) break;

            if (!LocalClient->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(0.1)))
            {
                continue;
            }

            uint8 HeaderBuffer[4] = { 0 };
            int32 BytesRead = 0;
            int32 BytesReadTotal = 0;

            while (BytesReadTotal < 4 && bIsRunning && bClientConnected)
            {
                if (!LocalClient) break;

                if (LocalClient->Recv(HeaderBuffer + BytesReadTotal, 4 - BytesReadTotal, BytesRead))
                {
                    if (BytesRead > 0)
                    {
                        BytesReadTotal += BytesRead;
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("PCL Client Closed Connection (Header read 0 bytes)."));
                        bClientConnected = false;
                    }
                }
                else
                {
                    bClientConnected = false;
                }
            }

            if (!bIsRunning || !bClientConnected) break;

            uint32 FrameSize = 0;
            FMemory::Memcpy(&FrameSize, HeaderBuffer, 4);

            if (FrameSize == 0 || FrameSize > (uint32)MaxFrameBytes)
            {
                UE_LOG(LogTemp, Error, TEXT("Invalid PCL FrameSize (%u). Disconnecting."), FrameSize);
                bClientConnected = false;
                break;
            }

            TArray<uint8> FrameBuffer;
            FrameBuffer.SetNumUninitialized(FrameSize);

            BytesReadTotal = 0;
            while (BytesReadTotal < (int32)FrameSize && bIsRunning && bClientConnected)
            {
                if (!LocalClient) break;

                if (!LocalClient->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(2.0)))
                {
                    UE_LOG(LogTemp, Error, TEXT("PCL Body Recv Timeout."));
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

            if (!bIsRunning || !bClientConnected) break;

            EnqueueFrame(FrameBuffer);
            FrameCount++;
        }

        UE_LOG(LogTemp, Warning, TEXT("<<< PCL Client Disconnected >>>"));

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

    UE_LOG(LogTemp, Log, TEXT("PCL Server Thread Exited Gracefully."));
}
