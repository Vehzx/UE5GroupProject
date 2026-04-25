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

    // Enable mouse cursor for gameplay + UI
    bShowMouseCursor = true;

    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(false);
    InputMode.SetWidgetToFocus(nullptr);
    SetInputMode(InputMode);

    // Find the camera manager in the level
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraManager::StaticClass(), Found);
    if (Found.Num() > 0)
    {
        CameraActor = Cast<ACameraManager>(Found[0]);
        if (CameraActor)
        {
            SetViewTargetWithBlend(CameraActor, 0.f);
        }
    }

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

    // Build
    InputComponent->BindAction("BuildMode", IE_Pressed, this, &ATDPlayerController::StartBuildMode);
    InputComponent->BindAction("ConfirmBuild", IE_Pressed, this, &ATDPlayerController::HandleLeftClick);
    InputComponent->BindAction("CancelBuild", IE_Pressed, this, &ATDPlayerController::CancelBuildMode);
    InputComponent->BindAction("DebugKillPlayer", IE_Pressed, this, &ATDPlayerController::DebugKillPlayer);

    // Camera
    InputComponent->BindAction("ToggleCamera", IE_Pressed, this, &ATDPlayerController::ToggleCamera);
    InputComponent->BindAction("RotateCamera", IE_Pressed, this, &ATDPlayerController::StartCameraRotate);
    InputComponent->BindAction("RotateCamera", IE_Released, this, &ATDPlayerController::StopCameraRotate);
    InputComponent->BindAxis("MoveForward", this, &ATDPlayerController::OnMoveForward);
    InputComponent->BindAxis("MoveRight", this, &ATDPlayerController::OnMoveRight);
    InputComponent->BindAxis("Zoom", this, &ATDPlayerController::OnZoomInput);
    InputComponent->BindAxis("MouseX", this, &ATDPlayerController::OnMouseXInput);
    InputComponent->BindAxis("MouseY", this, &ATDPlayerController::OnMouseYInput);

    // Pause
    InputComponent->BindAction("Pause", IE_Pressed, this, &ATDPlayerController::TogglePauseMenu).bExecuteWhenPaused = true;
}

void ATDPlayerController::DeselectTower()
{
    // Hide the range before clearing the selection
    if (SelectedTower)
    {
        SelectedTower->SetRangeVisible(false);
    }

    SelectedTower = nullptr;

    if (HUDWidget)
    {
        HUDWidget->HideTowerUpgradePanel();
    }
}

void ATDPlayerController::HandleLeftClick()
{
    FHitResult Hit;
    GetHitResultUnderCursor(ECC_Visibility, false, Hit);

    // 1. If we are currently placing a tower → place it
    if (bIsPlacing)
    {
        ConfirmPlaceTower();
        return;
    }

    // 2. If we clicked a tower → select it
    if (ATowerBase* Tower = Cast<ATowerBase>(Hit.GetActor()))
    {
        OnTowerSelected(Tower);
        return;
    }

    // 3. Otherwise → clicked terrain → deselect
    DeselectTower();
}


void ATDPlayerController::ToggleCamera()
{
    if (CameraActor)
        CameraActor->ToggleCamera();
}

void ATDPlayerController::StartCameraRotate()
{
    bRotatingCamera = true;
}

void ATDPlayerController::StopCameraRotate()
{
    bRotatingCamera = false;
}

void ATDPlayerController::OnMoveForward(float Value)
{
    if (!CameraActor || Value == 0.f) return;

    if (CameraActor->bIsTopDown)
        CameraActor->MoveCamera(FVector(Value, 0.f, 0.f));
    else
        CameraActor->PanCamera3D(FVector(Value, 0.f, 0.f));
}

void ATDPlayerController::OnMoveRight(float Value)
{
    if (!CameraActor || Value == 0.f) return;

    if (CameraActor->bIsTopDown)
        CameraActor->MoveCamera(FVector(0.f, Value, 0.f));
    else
        CameraActor->PanCamera3D(FVector(0.f, Value, 0.f));
}

void ATDPlayerController::OnZoomInput(float Value)
{
    if (!CameraActor || Value == 0.f) return;

    if (CameraActor->bIsTopDown)
        CameraActor->ZoomCamera(Value);
    else
        CameraActor->Zoom3DCamera(Value);
}

void ATDPlayerController::OnMouseXInput(float Value)
{
    if (!CameraActor || !bRotatingCamera || CameraActor->bIsTopDown) return;
    CameraActor->RotateCamera(Value, 0.f);
}

void ATDPlayerController::OnMouseYInput(float Value)
{
    if (!CameraActor || !bRotatingCamera || CameraActor->bIsTopDown) return;
    CameraActor->RotateCamera(0.f, -Value);
}

void ATDPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    if (!bIsPlacing || !PreviewTower)
        return;

    FHitResult Hit;
    if (GetHitResultUnderCursor(ECC_Visibility, true, Hit))
    {
        // If we're hovering over the path, don't move the preview there
        if (Hit.GetActor() && Hit.GetActor()->ActorHasTag("Path"))
        {
            return;
        }

        FVector Loc = Hit.ImpactPoint;

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

        // If Delta is negative, it means we took damage
        if (Delta < 0.f)
        {
            HUD->PlayDamageFlash();

            // Trigger the camera shake!
            if (DamageCameraShake)
            {
                ClientStartCameraShake(DamageCameraShake);
            }
        }
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

void ATDPlayerController::UpdateWaveTimerHUD(float TimeRemaining)
{
    if (HUDWidget)
        HUDWidget->UpdateWaveTimer(TimeRemaining);
}

void ATDPlayerController::UpdateWaveNumberHUD(int32 WaveNumber, int32 TotalWaves)
{
    if (HUDWidget)
        HUDWidget->UpdateWaveNumber(WaveNumber, TotalWaves);
}

void ATDPlayerController::StartWaveCountdown(float Delay)
{
    WaveCountdownRemaining = Delay;
    GetWorldTimerManager().SetTimer(
        WaveCountdownHandle,
        this,
        &ATDPlayerController::TickWaveCountdown,
        1.0f,
        true
    );
}

void ATDPlayerController::NotifyNPCSpawned()
{
    AliveNPCs++;
}

void ATDPlayerController::NotifyNPCDied()
{
    AliveNPCs--;

    CheckForWinCondition();
}

void ATDPlayerController::NotifyAllWavesSpawned()
{
    bAllWavesSpawned = true;

    CheckForWinCondition();
}

void ATDPlayerController::CheckForWinCondition()
{
    if (bAllWavesSpawned && AliveNPCs <= 0)
    {
        OnWin();
    }
}


void ATDPlayerController::TickWaveCountdown()
{
    WaveCountdownRemaining -= 1.f;

    if (HUDWidget)
        HUDWidget->UpdateWaveTimer(FMath::Max(0.f, WaveCountdownRemaining));

    if (WaveCountdownRemaining <= 0.f)
        GetWorldTimerManager().ClearTimer(WaveCountdownHandle);
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
        PreviewTower->SetRangeVisible(true);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("StartBuildMode: FAILED to spawn PreviewTower"));
    }
}

void ATDPlayerController::OnTowerSelected(ATowerBase* Tower)
{
    // Hide the old tower's range if we already had one selected
    if (SelectedTower)
    {
        SelectedTower->SetRangeVisible(false);
    }

    SelectedTower = Tower;

    // Show the new tower's range
    if (SelectedTower)
    {
        SelectedTower->SetRangeVisible(true);
    }

    if (HUDWidget)
    {
        HUDWidget->ShowTowerUpgradePanel(Tower);
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

    // Block placement on path
    if (Hit.GetActor() && Hit.GetActor()->ActorHasTag("Path"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot place tower on path"));
        return;
    }

    FVector PlaceLoc = Hit.ImpactPoint;

    PlaceLoc.X = FMath::GridSnap(PlaceLoc.X, GridSize);
    PlaceLoc.Y = FMath::GridSnap(PlaceLoc.Y, GridSize);

    //PlaceLoc.Z = 100.f;

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
        FVector Origin, Extent;
        NewTower->GetActorBounds(true, Origin, Extent);
        float LowestPointZ = Origin.Z - Extent.Z;

        FVector Loc = NewTower->GetActorLocation();
        Loc.Z -= LowestPointZ;
        NewTower->SetActorLocation(Loc);

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

void ATDPlayerController::TogglePauseMenu()
{
    // If the game is already paused, unpause it
    if (UGameplayStatics::IsGamePaused(GetWorld()))
    {
        ResumeGame();
    }
    else
    {
        // Pause Game
        UGameplayStatics::SetGamePaused(GetWorld(), true);

        // Create the widget if it hasn't been created yet
        if (PauseMenuClass && !PauseMenuWidget)
        {
            PauseMenuWidget = CreateWidget<UUserWidget>(this, PauseMenuClass);
        }

        // Add to viewport and change input mode to UI so they can click buttons
        if (PauseMenuWidget && !PauseMenuWidget->IsInViewport())
        {
            PauseMenuWidget->AddToViewport();

            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
            SetInputMode(InputMode);
            bShowMouseCursor = true;
        }
    }
}

void ATDPlayerController::ResumeGame()
{
    // Unpause the game
    UGameplayStatics::SetGamePaused(GetWorld(), false);

    // Remove the pause menu from the screen
    if (PauseMenuWidget && PauseMenuWidget->IsInViewport())
    {
        PauseMenuWidget->RemoveFromParent();
    }

    // Return input mode back to normal gameplay
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(false);
    InputMode.SetWidgetToFocus(nullptr);
    SetInputMode(InputMode);
    bShowMouseCursor = true;
}