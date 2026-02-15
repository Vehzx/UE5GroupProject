#include "TDPlayerController.h"
#include "TowerBase.h"
#include "Engine/World.h"

ATDPlayerController::ATDPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void ATDPlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void ATDPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("BuildMode", IE_Pressed, this, &ATDPlayerController::StartBuildMode);
    InputComponent->BindAction("ConfirmBuild", IE_Pressed, this, &ATDPlayerController::ConfirmPlaceTower);
    InputComponent->BindAction("CancelBuild", IE_Pressed, this, &ATDPlayerController::CancelBuildMode);
}

void ATDPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    if (!bIsPlacing || !PreviewTower)
        return;

    FHitResult Hit;
    if (GetHitResultUnderCursor(ECC_Visibility, true, Hit))
    {
        FVector Loc = Hit.ImpactPoint;

        // Grid snap
        if (GridSize > 0.f)
        {
            Loc.X = FMath::GridSnap(Loc.X, GridSize);
            Loc.Y = FMath::GridSnap(Loc.Y, GridSize);
        }

        PreviewTower->SetActorLocation(Loc);
    }
}

void ATDPlayerController::StartBuildMode()
{
    if (!TowerToBuild)
        return;

    CancelBuildMode();
    bIsPlacing = true;

    FHitResult Hit;
    if (!GetHitResultUnderCursor(ECC_Visibility, true, Hit))
    {
        bIsPlacing = false;
        return;
    }

    FVector SpawnLoc = Hit.ImpactPoint;

    if (GridSize > 0.f)
    {
        SpawnLoc.X = FMath::GridSnap(SpawnLoc.X, GridSize);
        SpawnLoc.Y = FMath::GridSnap(SpawnLoc.Y, GridSize);
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    PreviewTower = GetWorld()->SpawnActor<ATowerBase>(
        TowerToBuild,
        SpawnLoc,
        FRotator::ZeroRotator,
        Params
    );

    if (PreviewTower)
    {
        PreviewTower->SetPreview(true);
        PreviewTower->SetActorHiddenInGame(false);
    }
}

void ATDPlayerController::CancelBuildMode()
{
    bIsPlacing = false;

    if (PreviewTower)
    {
        PreviewTower->Destroy();
        PreviewTower = nullptr;
    }
}

void ATDPlayerController::ConfirmPlaceTower()
{
    if (!bIsPlacing || !PreviewTower || !TowerToBuild)
        return;

    const FVector PlaceLoc = PreviewTower->GetActorLocation();

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ATowerBase* NewTower = GetWorld()->SpawnActor<ATowerBase>(
        TowerToBuild,
        PlaceLoc,
        FRotator::ZeroRotator,
        Params
    );

    if (NewTower)
    {
        NewTower->SetPreview(false);
    }

    CancelBuildMode();
}
