#include "VRPoseTracker.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"

UVRPoseTracker::UVRPoseTracker()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UVRPoseTracker::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	VRCameraComponent = Owner->FindComponentByClass<UCameraComponent>();
	if (!VRCameraComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("VRPoseTracker: Camera component not found."));
	}
}