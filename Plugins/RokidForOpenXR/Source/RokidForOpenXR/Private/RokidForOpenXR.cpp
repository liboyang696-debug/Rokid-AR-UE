// Copyright Epic Games, Inc. All Rights Reserved.

#include "RokidForOpenXR.h"

void FRokidForOpenXRModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogTemp, Log, TEXT("FRokidForOpenXRModule StartupModule()"));
	RokidForOpenXRPlugin.Register();
	RokidForOpenXRHand.Register();
	RokidForOpenXRController.Register();
}

void FRokidForOpenXRModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogTemp, Warning, TEXT("FRokidForOpenXRModule ShutdownModule()"));
	RokidForOpenXRPlugin.Unregister();
	RokidForOpenXRHand.Unregister();
	RokidForOpenXRController.Unregister();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRokidForOpenXRModule, RokidForOpenXR)