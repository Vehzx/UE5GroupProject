#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageTextWidget.generated.h"

class UTextBlock;

UCLASS()
class UE5GROUPPROJECT_API UDamageTextWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetDamageText(float Damage);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextDamage;
};