#include "VRBasePawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

AVRBasePawn::AVRBasePawn()
{
    PrimaryActorTick.bCanEverTick = true;
    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
}

void AVRBasePawn::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (DefaultMappingContext) Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    CachedVRCamera = FindComponentByClass<UCameraComponent>();

    TArray<AActor*> FoundScreens;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("MainScreen"), FoundScreens);
    if (FoundScreens.Num() > 0) CachedScreenActor = FoundScreens[0];

    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Anchor"), AnchorActors);
    AnchorActors.Sort([](const AActor& A, const AActor& B) {
        return A.GetName() < B.GetName();
        });

}

void AVRBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction) EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVRBasePawn::Move);
        if (JumpNextAction) EIC->BindAction(JumpNextAction, ETriggerEvent::Started, this, &AVRBasePawn::JumpNext);
        if (JumpPrevAction) EIC->BindAction(JumpPrevAction, ETriggerEvent::Started, this, &AVRBasePawn::JumpPrev);
    }
}

void AVRBasePawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    PoseLogTimer += DeltaTime;
    if (PoseLogTimer < 1.0f)
    {
        return;
    }
    PoseLogTimer = 0.0f;

    const FVector PawnLocation = GetActorLocation();
    const FRotator PawnRotation = GetActorRotation();

    if (CachedVRCamera)
    {
        const FVector CameraLocation = CachedVRCamera->GetComponentLocation();
        const FRotator CameraRotation = CachedVRCamera->GetComponentRotation();
        const FVector CameraRelativeLocation = CachedVRCamera->GetRelativeLocation();
        const FRotator CameraRelativeRotation = CachedVRCamera->GetRelativeRotation();

        UE_LOG(LogTemp, Log, TEXT("[VRPawn Pose] PawnLoc=%s PawnRot=%s CameraWorldLoc=%s CameraWorldRot=%s CameraRelLoc=%s CameraRelRot=%s"),
            *PawnLocation.ToString(),
            *PawnRotation.ToString(),
            *CameraLocation.ToString(),
            *CameraRotation.ToString(),
            *CameraRelativeLocation.ToString(),
            *CameraRelativeRotation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[VRPawn Pose] PawnLoc=%s PawnRot=%s Camera=NULL"),
            *PawnLocation.ToString(),
            *PawnRotation.ToString());
    }
}

void AVRBasePawn::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    if (!MovementVector.IsZero() && Controller && CachedVRCamera)
    {
        FRotator YawRotation(0, CachedVRCamera->GetComponentRotation().Yaw, 0);
        AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), MovementVector.Y);
        AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), MovementVector.X);
    }
}

void AVRBasePawn::JumpNext()
{
    if (AnchorActors.Num() > 0) PerformJump((CurrentAnchorIndex + 1) % AnchorActors.Num());
}

void AVRBasePawn::JumpPrev()
{
    if (AnchorActors.Num() > 0) PerformJump((CurrentAnchorIndex <= 0) ? AnchorActors.Num() - 1 : CurrentAnchorIndex - 1);
}

void AVRBasePawn::PerformJump(int32 TargetIndex)
{
    if (!AnchorActors.IsValidIndex(TargetIndex)) return;

    CurrentAnchorIndex = TargetIndex;
    AActor* TargetAnchor = AnchorActors[CurrentAnchorIndex];
    if (!TargetAnchor) return;

    SetActorLocation(TargetAnchor->GetActorLocation());

    if (CachedScreenActor)
    {
        FVector NewScreenLoc = TargetAnchor->GetActorLocation() + (TargetAnchor->GetActorForwardVector() * 360.0f);
        FRotator NewScreenRot = TargetAnchor->GetActorRotation();
        NewScreenRot.Yaw += 180.0f;
        CachedScreenActor->SetActorLocationAndRotation(NewScreenLoc, NewScreenRot);
    }
}
