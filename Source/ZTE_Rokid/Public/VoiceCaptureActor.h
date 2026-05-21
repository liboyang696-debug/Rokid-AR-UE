#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoiceCaptureActor.generated.h"

UCLASS()
class ZTE_ROKID_API AVoiceCaptureActor : public AActor
{
	GENERATED_BODY()

public:

	AVoiceCaptureActor();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

};
