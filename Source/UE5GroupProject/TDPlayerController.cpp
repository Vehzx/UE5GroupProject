#include "TDPlayerController.h"
#include "TowerBase.h"
#include "Engine/World.h"
#include "PlayerStatsComponent.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameOverWidget.h"
#include "PlayerHUDWidget.h"

ATDPlayerController::ATDPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    PlayerStats = CreateDefaultSubobject<UPlayerStatsComponent>(TEXT("PlayerStats"));
}

void ATDPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Restore normal game input after level restart
    SetInputMode(FInputModeGameOnly());
    SetShowMouseCursor(false);

    // -----------------------------
    // Create and store the HUD widget
    // -----------------------------
    if (PlayerHUDClass)
    {
        HUDWidget = Cast<UPlayerHUDWidget>(CreateWidget(this, PlayerHUDClass));

        if (HUDWidget)
        {
            HUDWidget->AddToViewport();

            // -----------------------------
            // Initialize HUD with current stats
            // -----------------------------
            if (PlayerStats)
            {
                HUDWidget->UpdateHealth(PlayerStats->CurrentHealth);
                HUDWidget->UpdateGold(PlayerStats->CurrentGold);
            }
        }
    }

    // -----------------------------
    // Bind PlayerStats events
    // -----------------------------
    if (PlayerStats)
    {
        PlayerStats->OnPlayerDeath.AddDynamic(this, &ATDPlayerController::HandlePlayerDeath);
        PlayerStats->OnHealthChanged.AddDynamic(this, &ATDPlayerController::OnHealthChanged);
        PlayerStats->OnGoldChanged.AddDynamic(this, &ATDPlayerController::OnGoldChanged);
    }
}

UPlayerHUDWidget* ATDPlayerController::GetHUDWidget() const
{
    return HUDWidget;
}


void ATDPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("BuildMode", IE_Pressed, this, &ATDPlayerController::StartBuildMode);
    InputComponent->BindAction("ConfirmBuild", IE_Pressed, this, &ATDPlayerController::ConfirmPlaceTower);
    InputComponent->BindAction("CancelBuild", IE_Pressed, this, &ATDPlayerController::CancelBuildMode);
    InputComponent->BindAction("DebugKillPlayer", IE_Pressed, this, &ATDPlayerController::DebugKillPlayer);
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

void ATDPlayerController::OnHealthChanged(float NewHealth, float Delta)
{
    if (auto HUD = Cast<UPlayerHUDWidget>(GetHUDWidget()))
    {
        HUD->UpdateHealth(NewHealth);
    }
}

void ATDPlayerController::OnGoldChanged(int32 NewGold, int32 Delta)
{
    if (auto HUD = Cast<UPlayerHUDWidget>(GetHUDWidget()))
    {
        HUD->UpdateGold(NewGold);
    }
}

void ATDPlayerController::DebugKillPlayer()
{
    if (PlayerStats)
    {
        PlayerStats->ApplyDamageToPlayer(9999.f);
    }
}

void ATDPlayerController::HandlePlayerDeath()
{
    UE_LOG(LogTemp, Warning, TEXT("Player died — showing Game Over screen"));

    UGameplayStatics::SetGamePaused(GetWorld(), true);

    if (GameOverScreenClass)
    {
        UUserWidget* Widget = CreateWidget<UUserWidget>(this, GameOverScreenClass);
        if (Widget)
        {
            Widget->AddToViewport();
            SetShowMouseCursor(true);
            SetInputMode(FInputModeUIOnly());

            // -----------------------------
            // Pass stats to Game Over UI
            // -----------------------------
            if (PlayerStats)
            {
                // Cast to your typed widget class
                if (UGameOverWidget* TypedWidget = Cast<UGameOverWidget>(Widget))
                {
                    TypedWidget->SetGameOverStats(
                        PlayerStats->GoldEarned,
                        PlayerStats->GoldSpent,
                        PlayerStats->EnemiesKilled
                    );
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("GameOverScreenClass is NOT a UGameOverWidget!"));
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GameOverScreenClass is NOT set in PlayerController!"));
    }
}

void ATDPlayerController::RestartGame()
{
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
}

void ATDPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsPlacing && PreviewTower)
    {
        FVector Loc = PreviewTower->GetActorLocation();
        Loc.Z = 100.f; // or whatever your landscape height is
        PreviewTower->SetActorLocation(Loc);
    }
}

void ATDPlayerController::StartBuildMode()
{
    UE_LOG(LogTemp, Warning, TEXT("StartBuildMode called"));

    if (!TowerToBuild)
    {
        UE_LOG(LogTemp, Error, TEXT("StartBuildMode: TowerToBuild is NULL"));
        return;
    }

    CancelBuildMode();
    bIsPlacing = true;

    FHitResult Hit;
    if (!GetHitResultUnderCursor(ECC_Visibility, true, Hit))
    {
        UE_LOG(LogTemp, Error, TEXT("StartBuildMode: GetHitResultUnderCursor failed"));
        bIsPlacing = false;
        return;
    }

    FVector SpawnLoc = Hit.ImpactPoint;

    // Grid snap
    if (GridSize > 0.f)
    {
        SpawnLoc.X = FMath::GridSnap(SpawnLoc.X, GridSize);
        SpawnLoc.Y = FMath::GridSnap(SpawnLoc.Y, GridSize);
    }

    // SAFETY CLAMP
    if (SpawnLoc.Z < 99.f)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnLoc was below world! Clamping from %f to 100."), SpawnLoc.Z);
        SpawnLoc.Z = 100.f;
    }

    UE_LOG(LogTemp, Warning, TEXT("StartBuildMode: Intended SpawnLoc.Z = %f"), SpawnLoc.Z);

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
        // Disable collision so Unreal stops pushing the actor down
        PreviewTower->SetActorEnableCollision(false);
        PreviewTower->TowerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        // Now compute bounds
        FVector Origin, Extent;
        PreviewTower->GetActorBounds(true, Origin, Extent);

        // Lowest point of the mesh in world space
        float LowestPointZ = Origin.Z - Extent.Z;

        // Lift actor so lowest point sits exactly on the ground
        FVector Loc = PreviewTower->GetActorLocation();
        Loc.Z -= LowestPointZ;
        PreviewTower->SetActorLocation(Loc);

        UE_LOG(LogTemp, Warning, TEXT("Preview adjusted: LowestPointZ=%f FinalZ=%f"), LowestPointZ, Loc.Z);

        PreviewTower->SetPreview(true);
        PreviewTower->SetActorHiddenInGame(false);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("StartBuildMode: FAILED to spawn PreviewTower"));
    }
}

void ATDPlayerController::ConfirmPlaceTower()
{
    UE_LOG(LogTemp, Warning, TEXT("ConfirmPlaceTower called"));

    // Validate state
    if (!bIsPlacing || !PreviewTower || !TowerToBuild)
    {
        UE_LOG(LogTemp, Error, TEXT("ConfirmPlaceTower: invalid state. bIsPlacing=%d, PreviewTower=%s, TowerToBuild=%s"),
            bIsPlacing ? 1 : 0,
            *GetNameSafe(PreviewTower),
            *GetNameSafe(TowerToBuild.Get())
        );
        return;
    }

    // -----------------------------
    //  Tower Cost Check
    // -----------------------------
    const int32 TowerCost = 50;

    if (!PlayerStats || !PlayerStats->SpendGold(TowerCost))
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough gold to place tower!"));
        CancelBuildMode(); // Remove preview
        return;
    }

    // -----------------------------
    //  Get placement location
    // -----------------------------
    FHitResult Hit;
    if (!GetHitResultUnderCursor(ECC_Visibility, true, Hit))
    {
        UE_LOG(LogTemp, Error, TEXT("ConfirmPlaceTower: Failed ground trace"));
        return;
    }

    FVector PlaceLoc = Hit.ImpactPoint;
    PlaceLoc.Z = 100.f; // Your ground baseline

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // -----------------------------
    //  Spawn the actual tower
    // -----------------------------
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
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ConfirmPlaceTower: FAILED to spawn NewTower"));
        // Refund gold if spawn failed
        PlayerStats->AddGold(TowerCost);
    }

    CancelBuildMode();
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