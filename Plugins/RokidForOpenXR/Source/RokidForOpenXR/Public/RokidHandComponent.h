// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PoseableMeshComponent.h"
#include "HeadMountedDisplayTypes.h"
#include "RokidHandComponent.generated.h"

UENUM(BlueprintType)
enum class ERokidHandType : uint8
{
	HandLeft,
	HandRight,
};

static const TArray<FName> RK_LEFT_HAND_BONE_NAMES = {
	TEXT("left_palm"), TEXT("left_wrist"),
	TEXT("left_thumb_metacarpal"), TEXT("left_thumb_proximal"), TEXT("left_thumb_distal"), TEXT("left_thumb_tip"),
	TEXT(""), TEXT("left_index_proximal"), TEXT("left_index_intermediate"), TEXT("left_index_distal"), TEXT("left_index_tip"),
	TEXT(""), TEXT("left_middle_proximal"), TEXT("left_middle_intermediate"), TEXT("left_middle_distal"), TEXT("left_middle_tip"),
	TEXT(""), TEXT("left_ring_proximal"), TEXT("left_ring_intermediate"), TEXT("left_ring_distal"), TEXT("left_ring_tip"),
	TEXT(""), TEXT("left_little_proximal"), TEXT("left_little_intermediate"), TEXT("left_little_distal"), TEXT("left_little_tip")
};

UCLASS(Blueprintable, ClassGroup=(RokidComponent), meta=(BlueprintSpawnableComponent))
class ROKIDFOROPENXR_API URokidHandComponent : public UPoseableMeshComponent
{
	GENERATED_UCLASS_BODY()

public:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Which type of the skeleton mesh, left hand or right hand. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rokid HandTracking")
	ERokidHandType SkeletonMeshType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rokid HandTracking")
	bool ApplyLocationToEveryBone;
	
	/** Whether to automatically hide this component based on the validity of the hand tracking data. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rokid HandTracking")
	bool AutoHide;

	/** Bone mapping for custom hand skeletal meshes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rokid HandTracking")
	TMap<EHandKeypoint, FName> BoneMappings;

private:
	bool bHandTrackingAvailable;
};
