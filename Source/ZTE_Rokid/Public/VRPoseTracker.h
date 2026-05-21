#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VRPoseTracker.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZTE_ROKID_API UVRPoseTracker : public UActorComponent
{
	GENERATED_BODY()

public:
	UVRPoseTracker();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class UCameraComponent* VRCameraComponent;
};