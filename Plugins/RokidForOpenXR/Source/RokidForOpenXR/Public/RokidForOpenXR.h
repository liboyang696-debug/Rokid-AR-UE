// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "RokidForOpenXRController.h"
#include "RokidForOpenXRHand.h"
#include "RokidForOpenXRPlugin.h"

class FRokidForOpenXRModule : public IModuleInterface
{
public:
	static inline FRokidForOpenXRModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FRokidForOpenXRModule>("RokidForOpenXR");
	}
	FRokidForOpenXRPlugin& GetXRPlugin() { return RokidForOpenXRPlugin; }

	FRokidForOpenXRController& GetControllerPlugin() { return RokidForOpenXRController; }
	
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	FRokidForOpenXRPlugin RokidForOpenXRPlugin;
	FRokidForOpenXRHand RokidForOpenXRHand;
	FRokidForOpenXRController RokidForOpenXRController;
};
