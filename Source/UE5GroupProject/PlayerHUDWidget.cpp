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