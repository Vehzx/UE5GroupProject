#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

UCLASS()
class UE5GROUPPROJECT_API UPlayerHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable)
    void UpdateHealth(float NewHealth);

    UFUNCTION(BlueprintCallable)
    void UpdateGold(int32 NewGold);

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* HealthText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* GoldText;
};