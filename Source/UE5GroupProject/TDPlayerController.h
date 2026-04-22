#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerStatsComponent.h"
#include "CameraManager.h"
#include "TDPlayerController.generated.h"


class ATowerBase;

UCLASS()
class UE5GROUPPROJECT_API ATDPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ATDPlayerController();

    UPROPERTY()
    ACameraManager* CameraActor;

    void ToggleCamera();
    void OnZoomInput(float Value);
    void OnMouseXInput(float Value);
    void OnMouseYInput(float Value);

    // Camera pan inputs
    void OnMoveForward(float Value);
    void OnMoveRight(float Value);

    bool bRotatingCamera = false;
    void StartCameraRotate();
    void StopCameraRotate();

    void DeselectTower();
    void HandleLeftClick();

    UFUNCTION()
    void OnTowerSelected(ATowerBase* Tower);

    UPROPERTY()
    ATowerBase* SelectedTower;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UPlayerStatsComponent* PlayerStats;

    UFUNCTION()
    void HandlePlayerDeath();

    UFUNCTION()
    void DebugKillPlayer();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> GameOverScreenClass;

    UFUNCTION(BlueprintCallable)
    void RestartGame();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> PlayerHUDClass;

    UFUNCTION()
    void OnHealthChanged(float NewHealth, float Delta);

    UFUNCTION()
    void OnGoldChanged(int32 NewGold, int32 Delta);

    UFUNCTION()
    UPlayerHUDWidget* GetHUDWidget() const;

    UPlayerHUDWidget* HUDWidget;

    UFUNCTION(BlueprintCallable)
    void StartWaveCountdown(float Delay);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> WinScreenClass;

    UFUNCTION(BlueprintImplementableEvent)
    void OnWin();

    void NotifyNPCSpawned();
    void NotifyNPCDied();
    UFUNCTION(BlueprintCallable)
    void NotifyAllWavesSpawned();
    void CheckForWinCondition();

protected:
    virtual void BeginPlay() override;
    virtual void PlayerTick(float DeltaTime) override;
    virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    // Set this in editor to choose which tower to place
    UPROPERTY(EditDefaultsOnly, Category = "Build")
    TSubclassOf<ATowerBase> TowerToBuild;

    int32 AliveNPCs = 0;
    bool bAllWavesSpawned = false;

    // The preview tower actor
    UPROPERTY()
    ATowerBase* PreviewTower = nullptr;

    bool bIsPlacing = false;

    void StartBuildMode();
    void CancelBuildMode();
    void ConfirmPlaceTower();

    //placement settings
    UPROPERTY(EditDefaultsOnly, Category = "Build")
    float GridSize = 200.f;

    UFUNCTION()
    void UpdateWaveTimerHUD(float TimeRemaining);

    UFUNCTION(BlueprintCallable)
    void UpdateWaveNumberHUD(int32 WaveNumber, int32 TotalWaves);

    FTimerHandle WaveCountdownHandle;
    float WaveCountdownRemaining = 0.f;
    void TickWaveCountdown();
};