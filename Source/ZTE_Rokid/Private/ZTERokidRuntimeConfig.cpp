#include "ZTERokidRuntimeConfig.h"

#include "Dom/JsonObject.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
const TCHAR* ConfigDirectoryName = TEXT("IpConfigFiles");
const TCHAR* ConfigFileName = TEXT("ipconfig.json");

FString GetDefaultConfigText()
{
	return TEXT("{\n")
		TEXT("  \"video_receiver\": {\n")
		TEXT("    \"ip\": \"0.0.0.0\",\n")
		TEXT("    \"port\": 4435\n")
		TEXT("  },\n")
		TEXT("  \"pose_sender\": {\n")
		TEXT("    \"ip\": \"10.168.1.125\",\n")
		TEXT("    \"port\": 9000\n")
		TEXT("  },\n")
		TEXT("  \"pcl_receiver\": {\n")
		TEXT("    \"ip\": \"0.0.0.0\",\n")
		TEXT("    \"port\": 4436\n")
		TEXT("  }\n")
		TEXT("}\n");
}

bool EnsureConfigFileExists(const FString& FilePath)
{
	if (FPaths::FileExists(FilePath))
	{
		return true;
	}

	const FString Directory = FPaths::GetPath(FilePath);
	if (!IFileManager::Get().MakeDirectory(*Directory, true))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create config directory: %s"), *Directory);
		return false;
	}

	const FString TemplatePath = FPaths::Combine(FPaths::ProjectContentDir(), ConfigDirectoryName, ConfigFileName);
	FString ConfigText;
	if (!FFileHelper::LoadFileToString(ConfigText, *TemplatePath))
	{
		ConfigText = GetDefaultConfigText();
	}

	if (!FFileHelper::SaveStringToFile(ConfigText, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to write config file: %s"), *FilePath);
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Created runtime config file: %s"), *FilePath);
	return true;
}

bool LoadConfigObject(TSharedPtr<FJsonObject>& OutRootObject)
{
	const FString FilePath = FZTERokidRuntimeConfig::GetConfigFilePath();
	if (!EnsureConfigFileExists(FilePath))
	{
		return false;
	}

	FString JsonText;
	if (!FFileHelper::LoadFileToString(JsonText, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Load ipconfig.json failed: %s"), *FilePath);
		return false;
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
	if (!FJsonSerializer::Deserialize(Reader, OutRootObject) || !OutRootObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Deserialize ipconfig.json failed: %s"), *FilePath);
		return false;
	}

	return true;
}
}

FString FZTERokidRuntimeConfig::GetConfigFilePath()
{
	return FPaths::Combine(FPaths::ProjectSavedDir(), ConfigDirectoryName, ConfigFileName);
}

bool FZTERokidRuntimeConfig::LoadEndpoint(const TCHAR* SectionName, FString& OutIP, int32& OutPort)
{
	TSharedPtr<FJsonObject> RootObject;
	if (!LoadConfigObject(RootObject))
	{
		return false;
	}

	const TSharedPtr<FJsonObject>* SectionObject = nullptr;
	if (!RootObject->TryGetObjectField(SectionName, SectionObject) || !SectionObject || !SectionObject->IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Missing field in ipconfig.json: %s"), SectionName);
		return false;
	}

	double PortValue = 0.0;
	if (!(*SectionObject)->TryGetStringField(TEXT("ip"), OutIP) ||
		!(*SectionObject)->TryGetNumberField(TEXT("port"), PortValue))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid endpoint in ipconfig.json: %s"), SectionName);
		return false;
	}

	OutPort = FMath::RoundToInt(PortValue);
	return true;
}
