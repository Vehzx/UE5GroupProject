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

void UPlayerHUDWidget::UpdateWaveNumber(int32 WaveNumber, int32 TotalWaves)
{
    if (WaveNumberText)
        WaveNumberText->SetText(FText::FromString(
            FString::Printf(TEXT("Wave: %d / %d"), WaveNumber, TotalWaves)));
}