#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "A.generated.h"

UCLASS()
class ZTE_ROKID_API AA : public AActor
{
	GENERATED_BODY()

public:

	AA();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

};
