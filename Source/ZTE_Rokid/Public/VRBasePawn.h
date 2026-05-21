#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "VRBasePawn.generated.h"

UCLASS()
class ZTE_ROKID_API AVRBasePawn : public APawn
{
    GENERATED_BODY()

public:
    AVRBasePawn();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR Settings|Input")
    class UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR Settings|Input")
    class UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR Settings|Input")
    class UInputAction* JumpNextAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR Settings|Input")
    class UInputAction* JumpPrevAction;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    class UFloatingPawnMovement* MovementComponent;

private:
    void Move(const FInputActionValue& Value);
    void JumpNext();
    void JumpPrev();
    void PerformJump(int32 TargetIndex);

    UPROPERTY()
    class UCameraComponent* CachedVRCamera;

    UPROPERTY()
    AActor* CachedScreenActor;

    UPROPERTY()
    TArray<AActor*> AnchorActors;

    int32 CurrentAnchorIndex = -1;
    float PoseLogTimer = 0.0f;
};
