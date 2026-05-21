#pragma once

#include "CoreMinimal.h"

class FZTERokidRuntimeConfig
{
public:
	static FString GetConfigFilePath();
	static bool LoadEndpoint(const TCHAR* SectionName, FString& OutIP, int32& OutPort);
};
