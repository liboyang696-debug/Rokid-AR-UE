#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "AsyncPointCloudReceiver.generated.h"

UCLASS()
class ZTE_ROKID_API AAsyncPointCloudReceiver : public AActor
{
    GENERATED_BODY()

public:
    AAsyncPointCloudReceiver();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, Category = "Network")
    int32 ListenPort = 4436;

    UPROPERTY(EditAnywhere, Category = "Data")
    int32 MaxQueueSize = 10;

    UPROPERTY(EditAnywhere, Category = "Data")
    int32 MaxFrameBytes = 200 * 1024 * 1024;

    bool LoadIPConfig(FString& OutIP, int32& OutPort);

    UFUNCTION(BlueprintCallable, Category = "PointCloud")
    bool PopPointCloudFrame(TArray<uint8>& OutFrame);

    UFUNCTION(BlueprintCallable, Category = "PointCloud")
    int32 GetQueueSize() const;

private:

    FThreadSafeBool bIsRunning;
    FThreadSafeBool bClientConnected;

    mutable FCriticalSection DataGuard;
    TArray<TArray<uint8>> PointCloudQueue;

    FCriticalSection SocketGuard;
    FSocket* ListenerSocket;
    FSocket* ClientSocket;

    float NextUpdateTime = 0.0f;
    int32 FrameCount = 0;
    const float FpsInterval = 2.0f;

    void StartTCPServer();
    void Thread_ServerLoop();

    void EnqueueFrame(const TArray<uint8>& FrameBytes);
};
