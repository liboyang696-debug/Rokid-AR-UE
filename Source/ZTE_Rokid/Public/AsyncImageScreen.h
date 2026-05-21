#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"

#include "AsyncImageScreen.generated.h"

UCLASS()
class ZTE_ROKID_API AAsyncImageScreen : public AActor
{
    GENERATED_BODY()

public:
    AAsyncImageScreen();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Screen")
    UStaticMeshComponent* ScreenMesh;

    UPROPERTY()
    UMaterialInstanceDynamic* DynamicMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen")
    UMaterialInterface* BaseMaterial;

    UPROPERTY(EditAnywhere, Category = "Network")
    int32 ListenPort = 4435;
    bool LoadIPConfig(FString& OutIP, int32& OutPort);

private:

    UPROPERTY()
    UTexture2D* TargetTexture;
    FUpdateTextureRegion2D* TextureRegion;
    bool bIsFirstData;

    FThreadSafeBool bIsRunning;
    FThreadSafeBool bClientConnected;

    FCriticalSection DataGuard;
    TArray<TArray<uint8>> RgbBufferQueue;

    FCriticalSection SocketGuard;
    FSocket* ListenerSocket;
    FSocket* ClientSocket;

    void StartTCPServer();
    void Thread_ServerLoop();
    void ProcessRGBFrame(const TArray<uint8>& CompressedData);
    void ResetScreenForNextConnection();
    void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);
};
