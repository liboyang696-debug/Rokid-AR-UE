// Fill out your copyright notice in the Description page of Project Settings.

#include "RokidForOpenXRController.h"

#include "HeadMountedDisplayFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "FRokidForOpenXRController"

const FKey RokidController_Left_Select_Click("RokidController_Left_Select_Click");
const FKey RokidController_Left_X_Click("RokidController_Left_X_Click");
const FKey RokidController_Left_O_Click("RokidController_Left_O_Click");
const FKey RokidController_Left_Up_Click("RokidController_Left_Up_Click");
const FKey RokidController_Left_Down_Click("RokidController_Left_Down_Click");
const FKey RokidController_Left_Left_Click("RokidController_Left_Left_Click");
const FKey RokidController_Left_Right_Click("RokidController_Left_Right_Click");
const FKey RokidController_Left_Menu_Click("RokidController_Left_Menu_Click");

FRokidForOpenXRController::FRokidForOpenXRController()
{
}

void FRokidForOpenXRController::Register()
{
	UE_LOG(LogTemp, Log, TEXT("RokidForOpenXRController Register"));
	RegisterOpenXRExtensionModularFeature();

	EKeys::AddMenuCategoryDisplayInfo("Rokid", LOCTEXT("RokidSubCategory", "Rokid"), TEXT("GraphEditor.PadEvent_16x"));
	
	EKeys::AddKey(FKeyDetails(RokidController_Left_Select_Click, LOCTEXT("RokidController_Left_Select_Click", "Rokid Controller Select Press"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Rokid"));
	EKeys::AddKey(FKeyDetails(RokidController_Left_X_Click, LOCTEXT("RokidController_Left_X_Click", "Rokid Controller X Press"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Rokid"));
	EKeys::AddKey(FKeyDetails(RokidController_Left_O_Click, LOCTEXT("RokidController_Left_O_Click", "Rokid Controller O Press"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Rokid"));
	EKeys::AddKey(FKeyDetails(RokidController_Left_Up_Click, LOCTEXT("RokidController_Left_Up_Click", "Rokid Controller Up Press"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Rokid"));
	EKeys::AddKey(FKeyDetails(RokidController_Left_Down_Click, LOCTEXT("RokidController_Left_Down_Click", "Rokid Controller Down Press"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Rokid"));
	EKeys::AddKey(FKeyDetails(RokidController_Left_Left_Click, LOCTEXT("RokidController_Left_Left_Click", "Rokid Controller Left Press"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Rokid"));
	EKeys::AddKey(FKeyDetails(RokidController_Left_Right_Click, LOCTEXT("RokidController_Left_Right_Click", "Rokid Controller Right Press"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Rokid"));
	EKeys::AddKey(FKeyDetails(RokidController_Left_Menu_Click, LOCTEXT("RokidController_Left_Menu_Click", "Rokid Controller Menu Press"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Rokid"));
}

void FRokidForOpenXRController::Unregister()
{
	UE_LOG(LogTemp, Log, TEXT("RokidForOpenXRController Unregister"));
	UnregisterOpenXRExtensionModularFeature();
}

bool FRokidForOpenXRController::GetInteractionProfile(XrInstance InInstance, FString& OutKeyPrefix, XrPath& OutPath, bool& OutHasHaptics)
{
	UE_LOG(LogTemp, Log, TEXT("RokidForOpenXRController GetInteractionProfile /interaction_profiles/rokid/gamepad_controller."));

	XrResult Result = xrStringToPath(InInstance, "/interaction_profiles/rokid/gamepad_controller", &InteractionProfile);
	check(XR_SUCCEEDED(Result));
	OutKeyPrefix = "RokidController";
	OutPath = InteractionProfile;
	OutHasHaptics = false;
	return true;
}

const void* FRokidForOpenXRController::OnCreateSession(XrInstance InInstance, XrSystemId InSystem, const void* InNext)
{
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRecenterPhonePose", (PFN_xrVoidFunction*)&xrRecenterPhonePose));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetPhonePose", (PFN_xrVoidFunction*)&xrGetPhonePose));
	return InNext;
}

bool FRokidForOpenXRController::GetControllerPose(FVector& Position, FQuat& Rotation, FVector PositionOffset)
{
	if (xrGetPhonePose && UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected()) {
		float position[3];
		float orientation[4];
		XrResult res = xrGetPhonePose(position, orientation);
		if (res == XR_SUCCESS)
		{
			FRotator HmdRotation;
			FVector HmdPosition;
			UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HmdRotation, HmdPosition);
			Position.X = HmdPosition.X + PositionOffset.X;
			Position.Y = HmdPosition.Y + PositionOffset.Y;
			Position.Z = HmdPosition.Z + PositionOffset.Z;
			Rotation.X = -orientation[2];
			Rotation.Y = -orientation[0];
			Rotation.Z = -orientation[1];
			Rotation.W = orientation[3];
			return true;
		}
	}
	return false;
}

void FRokidForOpenXRController::RecenterControllerPose()
{
	if (xrRecenterPhonePose)
	{
		xrRecenterPhonePose();
	}
}


#undef LOCTEXT_NAMESPACE
