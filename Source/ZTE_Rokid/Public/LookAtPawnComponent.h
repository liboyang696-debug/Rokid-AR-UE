#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "LookAtPawnComponent.generated.h"

struct FUDPPlayerData
{
	float PosX;
	float PosY;
	float PosZ;
	float QuatX;
	float QuatY;
	float QuatZ;
	float QuatW;
};

class FUDPSenderWorker : public FRunnable
{
public:

	FUDPSenderWorker(FString InIP, int32 InPort);
	virtual ~FUDPSenderWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	void UpdateData(const FUDPPlayerData& NewData);

private:
	FRunnableThread* Thread;
	FSocket* Socket;
	TSharedPtr<FInternetAddr> RemoteAddr;
	FString TargetIP;
	int32 TargetPort;

	bool bRunThread;

	FUDPPlayerData CurrentData;
	FCriticalSection DataMutex;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZTE_ROKID_API ULookAtPawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULookAtPawnComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	AActor* PlayerPawn;

public:
	UPROPERTY(EditAnywhere, Category = "Settings")
	FRotator RotationOffset;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float InterpSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "UDP Settings")
	FString TargetIP = TEXT("10.168.1.125");

	UPROPERTY(EditAnywhere, Category = "UDP Settings")
	int32 TargetPort = 9000;
	bool LoadIPConfig(FString& OutIP, int32& OutPort);

private:

	FUDPSenderWorker* UDPWorker;
};
