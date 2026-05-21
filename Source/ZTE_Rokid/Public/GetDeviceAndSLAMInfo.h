#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GetDeviceAndSLAMInfo.generated.h"

UCLASS()
class ZTE_ROKID_API AGetDeviceAndSLAMInfo : public AActor
{
	GENERATED_BODY()

public:

	AGetDeviceAndSLAMInfo();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

};
