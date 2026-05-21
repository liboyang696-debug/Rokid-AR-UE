#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FRokidForOpenXRHand : public IOpenXRExtensionPlugin
{
public:
	FRokidForOpenXRHand();
	virtual ~FRokidForOpenXRHand() {}

	void Register();
	void Unregister();

	/** IOpenXRExtensionPlugin */
	virtual FString GetDisplayName() override
	{
		return FString(TEXT("RokidForOpenXRHand"));
	}
	virtual bool GetInteractionProfile(XrInstance InInstance, FString& OutKeyPrefix, XrPath& OutPath, bool& OutHasHaptics) override;

private:
	XrPath InteractionProfile;

};
