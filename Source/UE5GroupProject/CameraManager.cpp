#include "CameraManager.h"

ACameraManager::ACameraManager()
{
    PrimaryActorTick.bCanEverTick = true;

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    // Top-down orthographic camera — pointing straight down
    TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
    TopDownCamera->SetupAttachment(RootScene);
    TopDownCamera->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
    TopDownCamera->SetRelativeLocation(FVector(0.f, 0.f, 2000.f));
    TopDownCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
    TopDownCamera->OrthoWidth = 2048.f;

    // Spring arm for 3D perspective camera
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootScene);
    SpringArm->TargetArmLength = 2000.f;
    SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
    SpringArm->bDoCollisionTest = false;
    SpringArm->bInheritPitch = true;
    SpringArm->bInheritYaw = true;
    SpringArm->bInheritRoll = false;

    // 3D perspective camera
    PerspectiveCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PerspectiveCamera"));
    PerspectiveCamera->SetupAttachment(SpringArm);
    PerspectiveCamera->ProjectionMode = ECameraProjectionMode::Perspective;
}

void ACameraManager::BeginPlay()
{
    Super::BeginPlay();

    // Start with top-down active
    TopDownCamera->SetActive(true);
    PerspectiveCamera->SetActive(false);
}

void ACameraManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ACameraManager::ToggleCamera()
{
    bIsTopDown = !bIsTopDown;

    TopDownCamera->SetActive(bIsTopDown);
    PerspectiveCamera->SetActive(!bIsTopDown);
}

void ACameraManager::MoveCamera(FVector Direction)
{
    FVector NewLocation = GetActorLocation() + Direction * PanSpeed * GetWorld()->GetDeltaSeconds();
    SetActorLocation(NewLocation);
}

void ACameraManager::ZoomCamera(float Value)
{
    float NewOrthoWidth = TopDownCamera->OrthoWidth - Value * ZoomSpeed * GetWorld()->GetDeltaSeconds();
    TopDownCamera->OrthoWidth = FMath::Clamp(NewOrthoWidth, MinOrthoWidth, MaxOrthoWidth);
}

void ACameraManager::RotateCamera(float Yaw, float Pitch)
{
    FRotator CurrentRotation = SpringArm->GetRelativeRotation();
    float NewPitch = FMath::Clamp(CurrentRotation.Pitch + Pitch * RotateSpeed * GetWorld()->GetDeltaSeconds(), -80.f, -10.f);
    float NewYaw = CurrentRotation.Yaw + Yaw * RotateSpeed * GetWorld()->GetDeltaSeconds();
    SpringArm->SetRelativeRotation(FRotator(NewPitch, NewYaw, 0.f));
}

void ACameraManager::PanCamera3D(FVector Direction)
{
    FVector Forward = SpringArm->GetForwardVector() * Direction.X;
    FVector Right = SpringArm->GetRightVector() * Direction.Y;
    FVector Movement = (Forward + Right) * PanSpeed3D * GetWorld()->GetDeltaSeconds();
    Movement.Z = 0.f; // Keep movement horizontal
    SetActorLocation(GetActorLocation() + Movement);
}

void ACameraManager::Zoom3DCamera(float Value)
{
    float NewLength = SpringArm->TargetArmLength - Value * ZoomSpeed3D;
    SpringArm->TargetArmLength = FMath::Clamp(NewLength, MinArmLength, MaxArmLength);
}