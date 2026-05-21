// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FRokidForOpenXRController : public IOpenXRExtensionPlugin
{
public:
	FRokidForOpenXRController();
	virtual ~FRokidForOpenXRController() {}

	void Register();
	void Unregister();

	/** IOpenXRExtensionPlugin */
	virtual FString GetDisplayName() override
	{
		return FString(TEXT("RokidForOpenXRController"));
	}
	virtual bool GetInteractionProfile(XrInstance InInstance, FString& OutKeyPrefix, XrPath& OutPath, bool& OutHasHaptics) override;
	virtual const void* OnCreateSession(XrInstance InInstance, XrSystemId InSystem, const void* InNext) override;
	
	bool GetControllerPose(FVector& Position, FQuat& Rotation, FVector PositionOffset);
	void RecenterControllerPose();
	
private:
	XrPath InteractionProfile;
	PFN_xrRecenterPhonePose xrRecenterPhonePose;
	PFN_xrGetPhonePose xrGetPhonePose;
};
