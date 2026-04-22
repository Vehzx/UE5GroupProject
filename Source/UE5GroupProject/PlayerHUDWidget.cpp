#include "PlayerHUDWidget.h"
#include "Components/TextBlock.h"

void UPlayerHUDWidget::UpdateHealth(float NewHealth)
{
    if (HealthText)
        HealthText->SetText(FText::FromString(FString::Printf(TEXT("Health: %.0f"), NewHealth)));
}

void UPlayerHUDWidget::UpdateGold(int32 NewGold)
{
    if (GoldText)
        GoldText->SetText(FText::FromString(FString::Printf(TEXT("Gold: %d"), NewGold)));
}

void UPlayerHUDWidget::UpdateWaveTimer(float TimeRemaining)
{
    if (WaveTimerText)
    {
        if (TimeRemaining <= 0.f)
        {
            WaveTimerText->SetText(FText::FromString(TEXT("Wave Starting!")));
        }
        else
        {
            WaveTimerText->SetText(FText::FromString(
                FString::Printf(TEXT("Next Wave: %.0fs"), TimeRemaining)));
        }
    }
}

void UPlayerHUDWidget::ShowTowerUpgradePanel(ATowerBase* Tower)
{
    SelectedTower = Tower;

    // If you have a panel in the widget:
    if (TowerUpgradePanel)
    {
        TowerUpgradePanel->SetVisibility(ESlateVisibility::Visible);
    }

    // Update UI text here if needed
    UpdateTowerStats();
}

void UPlayerHUDWidget::HideTowerUpgradePanel()
{
    if (TowerUpgradePanel)
    {
        TowerUpgradePanel->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UPlayerHUDWidget::UpdateTowerStats()
{
    if (!SelectedTower)
        return;

    // Convert internal fire rate (seconds per shot) to shots per second
    const float ShotsPerSecond = (SelectedTower->FireRate > 0.f)
        ? 1.0f / SelectedTower->FireRate
        : 0.f;

    if (FireRateText)
    {
        FireRateText->SetText(
            FText::FromString(
                FString::Printf(TEXT("Upgrade Fire Rate (50G): %.1f /s"), ShotsPerSecond)
            )
        );
    }

    if (RangeText)
    {
        RangeText->SetText(
            FText::FromString(
                FString::Printf(TEXT("Upgrade Range (40G): %.0f"), SelectedTower->Range)
            )
        );
    }

    if (DamageText)
    {
        DamageText->SetText(
            FText::FromString(
                FString::Printf(TEXT("Upgrade Damage (60G): %.0f"), SelectedTower->TowerDamage)
            )
        );
    }
}

void UPlayerHUDWidget::UpdateWaveNumber(int32 WaveNumber, int32 TotalWaves)
{
    if (WaveNumberText)
        WaveNumberText->SetText(FText::FromString(
            FString::Printf(TEXT("Wave: %d / %d"), WaveNumber, TotalWaves)));
}