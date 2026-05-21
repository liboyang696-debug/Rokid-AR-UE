#include "LookAtPawnComponent.h"
#include "ZTERokidRuntimeConfig.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "Common/UdpSocketBuilder.h"

FUDPSenderWorker::FUDPSenderWorker(FString InIP, int32 InPort)
	: Thread(nullptr), Socket(nullptr), TargetIP(InIP), TargetPort(InPort), bRunThread(false)
{

	CurrentData = { 0,0,0, 0,0,0,1 };

	Thread = FRunnableThread::Create(this, TEXT("UDPSenderThread"), 0, TPri_BelowNormal);
}

FUDPSenderWorker::~FUDPSenderWorker()
{
	if (Thread)
	{
		Thread->Kill(true);
		delete Thread;
	}
}

bool FUDPSenderWorker::Init()
{
	bRunThread = true;

	Socket = FUdpSocketBuilder(TEXT("UDPSenderSocket"))
		.AsNonBlocking()
		.AsReusable()
		.WithBroadcast();

	if (Socket)
	{

		RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		bool bIsValid;
		RemoteAddr->SetIp(*TargetIP, bIsValid);
		RemoteAddr->SetPort(TargetPort);

		if (!bIsValid)
		{
			UE_LOG(LogTemp, Error, TEXT("UDP: Invalid IP address %s"), *TargetIP);
			return false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UDP: Failed to create socket."));
		return false;
	}

	return true;
}

uint32 FUDPSenderWorker::Run()
{

	while (bRunThread)
	{
		if (Socket && RemoteAddr.IsValid())
		{
			FUDPPlayerData DataToSend;

			{
				FScopeLock Lock(&DataMutex);
				DataToSend = CurrentData;
			}

			float Buffer[7];
			Buffer[0] = DataToSend.PosX;
			Buffer[1] = DataToSend.PosY;
			Buffer[2] = DataToSend.PosZ;
			Buffer[3] = DataToSend.QuatX;
			Buffer[4] = DataToSend.QuatY;
			Buffer[5] = DataToSend.QuatZ;
			Buffer[6] = DataToSend.QuatW;

			int32 BytesSent = 0;
			Socket->SendTo((uint8*)Buffer, sizeof(float) * 7, BytesSent, *RemoteAddr);
		}

		FPlatformProcess::Sleep(0.04f);
	}

	return 0;
}

void FUDPSenderWorker::Stop()
{
	bRunThread = false;
}

void FUDPSenderWorker::Exit()
{
	if (Socket)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		Socket = nullptr;
	}
}

void FUDPSenderWorker::UpdateData(const FUDPPlayerData& NewData)
{

	FScopeLock Lock(&DataMutex);
	CurrentData = NewData;
}

ULookAtPawnComponent::ULookAtPawnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.0f / 25.0f;
	RotationOffset = FRotator(0.0f, 90.0f, 90.0f);
	UDPWorker = nullptr;
}

bool ULookAtPawnComponent::LoadIPConfig(FString& OutIP, int32& OutPort)
{
	return FZTERokidRuntimeConfig::LoadEndpoint(TEXT("pose_sender"), OutIP, OutPort);
}

void ULookAtPawnComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	FString IP;
	int32 Port = 0;
	if (LoadIPConfig(IP, Port))
	{
		TargetIP = IP;
		TargetPort = Port;
		UE_LOG(LogTemp, Log, TEXT("Create Listener Socket on port %d"), TargetPort);
	}
	UDPWorker = new FUDPSenderWorker(TargetIP, TargetPort);
}

void ULookAtPawnComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	if (UDPWorker)
	{
		UDPWorker->Stop();

		delete UDPWorker;
		UDPWorker = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void ULookAtPawnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PlayerPawn))
	{
		PlayerPawn = PlayerController ? PlayerController->GetPawn() : UGameplayStatics::GetPlayerPawn(this, 0);
	}

	FTransform TargetTransform;
	bool bHasTargetTransform = false;

	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		TargetTransform = FTransform(
			PlayerController->PlayerCameraManager->GetCameraRotation(),
			PlayerController->PlayerCameraManager->GetCameraLocation());
		bHasTargetTransform = true;
	}
	else if (IsValid(PlayerPawn))
	{
		TargetTransform = PlayerPawn->GetActorTransform();
		bHasTargetTransform = true;
	}

	if (!bHasTargetTransform)
	{
		return;
	}

	FVector MyLoc = Owner->GetActorLocation();
	FVector TargetLoc = TargetTransform.GetLocation();

	TargetLoc.Z = MyLoc.Z;
	FVector DirectionAwayFromTarget = MyLoc - TargetLoc;

	if (!DirectionAwayFromTarget.IsNearlyZero())
	{
		FRotator LookAtRotation = DirectionAwayFromTarget.Rotation();
		LookAtRotation.Pitch = 0.0f;
		LookAtRotation.Roll = 0.0f;
		FRotator FinalTargetRotation = UKismetMathLibrary::ComposeRotators(RotationOffset, LookAtRotation);
		FRotator SmoothRotation = FMath::RInterpTo(Owner->GetActorRotation(), FinalTargetRotation, DeltaTime, InterpSpeed);
		Owner->SetActorRotation(SmoothRotation);
	}

	if (UDPWorker)
	{
		FTransform OwnerTransform = Owner->GetActorTransform();
		FTransform RelativeTransform = TargetTransform.GetRelativeTransform(OwnerTransform);

		FVector RelLoc = TargetTransform.GetLocation() - OwnerTransform.GetLocation();
		FQuat RelQuat = RelativeTransform.GetRotation();

		FUDPPlayerData Data;

		Data.PosX = RelLoc.Y / 100.0f;

		Data.PosY = 0.0f;

		Data.PosZ = RelLoc.X / 100.0f;

		Data.QuatX = RelQuat.Y;
		Data.QuatY = RelQuat.Z;
		Data.QuatZ = RelQuat.X;
		Data.QuatW = -RelQuat.W;

		UDPWorker->UpdateData(Data);
	}
}
