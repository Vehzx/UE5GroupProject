#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TowerBase.h"
#include "Components/TextBlock.h"
#include "PlayerHUDWidget.generated.h"

UCLASS()
class UE5GROUPPROJECT_API UPlayerHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void UpdateHealth(float NewHealth);

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWidget* TowerUpgradePanel;

    UPROPERTY(BlueprintReadWrite, Category = "Tower Upgrades")
    ATowerBase* SelectedTower;

    UFUNCTION(BlueprintCallable)
    void UpdateTowerStats();

    UFUNCTION(BlueprintCallable)
    void HideTowerUpgradePanel();

    UPROPERTY(meta = (BindWidget))
    UTextBlock* FireRateText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RangeText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DamageText;

    UFUNCTION(BlueprintCallable)
    void ShowTowerUpgradePanel(ATowerBase* Tower);

    UFUNCTION(BlueprintCallable)
    void UpdateGold(int32 NewGold);

    UFUNCTION(BlueprintCallable)
    void UpdateWaveTimer(float TimeRemaining);

    UFUNCTION(BlueprintCallable)
    void UpdateWaveNumber(int32 WaveNumber, int32 TotalWaves);

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* HealthText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* GoldText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* WaveTimerText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* WaveNumberText;
};