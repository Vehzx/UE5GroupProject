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