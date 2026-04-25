#include "DamageTextWidget.h"
#include "Components/TextBlock.h"

void UDamageTextWidget::SetDamageText(float Damage)
{
    if (TextDamage)
    {
        // Rounds the float to a whole number and converts it to a string
        FString DamageString = FString::FromInt(FMath::RoundToInt(Damage));
        TextDamage->SetText(FText::FromString(DamageString));
    }
}