// Fill out your copyright notice in the Description page of Project Settings.

#include "RokidForOpenXRHand.h"

#define LOCTEXT_NAMESPACE "FRokidForOpenXRHand"

// Left
const FKey RokidHandInteraction_Left_Grip("RokidHandInteraction_Left_Squeeze_Click");
const FKey RokidHandInteraction_Left_Pinch("RokidHandInteraction_Left_Pinch_Click");
//Right
const FKey RokidHandInteraction_Right_Grip("RokidHandInteraction_Right_Squeeze_Click");
const FKey RokidHandInteraction_Right_Pinch("RokidHandInteraction_Right_Pinch_Click");

FRokidForOpenXRHand::FRokidForOpenXRHand()
{
}

void FRokidForOpenXRHand::Register()
{
	UE_LOG(LogTemp, Log, TEXT("RokidForOpenXRHand Register"));
	RegisterOpenXRExtensionModularFeature();

	EKeys::AddMenuCategoryDisplayInfo("Rokid", LOCTEXT("RokidSubCategory", "Rokid"), TEXT("GraphEditor.PadEvent_16x"));

	// Left
	EKeys::AddKey(FKeyDetails(RokidHandInteraction_Left_Grip, LOCTEXT("RokidHandInteraction_Left_Grip_Click", "Rokid Left Hand Grip"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Rokid"));
	EKeys::AddKey(FKeyDetails(RokidHandInteraction_Left_Pinch, LOCTEXT("RokidHandInteraction_Left_Pinch_Click", "Rokid Left Hand Trigger"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Rokid"));
	// Right
	EKeys::AddKey(FKeyDetails(RokidHandInteraction_Right_Grip, LOCTEXT("RokidHandInteraction_Right_Grip_Click", "Rokid Right Hand Grip"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Rokid"));
	EKeys::AddKey(FKeyDetails(RokidHandInteraction_Right_Pinch, LOCTEXT("RokidHandInteraction_Right_Pinch_Click", "Rokid Right Hand Trigger"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Rokid"));
}

void FRokidForOpenXRHand::Unregister()
{
	UE_LOG(LogTemp, Log, TEXT("RokidForOpenXRHand Unregister"));
	UnregisterOpenXRExtensionModularFeature();
}

bool FRokidForOpenXRHand::GetInteractionProfile(XrInstance InInstance, FString& OutKeyPrefix, XrPath& OutPath, bool& OutHasHaptics)
{
	UE_LOG(LogTemp, Log, TEXT("RokidForOpenXRHand GetInteractionProfile /interaction_profiles/rokid/hand_interaction."));

	XrResult Result = xrStringToPath(InInstance, "/interaction_profiles/rokid/hand_interaction", &InteractionProfile);
	check(XR_SUCCEEDED(Result));
	OutKeyPrefix = "RokidHandInteraction";
	OutPath = InteractionProfile;
	OutHasHaptics = false;
	return true;
}

#undef LOCTEXT_NAMESPACE