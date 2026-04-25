#pragma once

#include "CoreMinimal.h"
#include "IDamageable.h"
#include "GameFramework/Character.h"
#include "GoldManager.h"
#include "NPCBase.generated.h"

class UDamageTextWidget;

UCLASS()
class UE5GROUPPROJECT_API ANPCBase : public ACharacter, public IDamageable
{
    GENERATED_BODY()

public:
    ANPCBase();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Stats")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC|Stats")
    float CurrentHealth;

    // Explicit interface implementation
    virtual void ApplyDamage(float DamageAmount) override;

    // Blueprint event for visual effects
    UFUNCTION(BlueprintNativeEvent)
    void OnDeath();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    class UWidgetComponent* HealthWidgetComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UDamageTextWidget> DamageTextClass;

    UFUNCTION(BlueprintCallable)
    void HandleReachedGoal();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    AGoldManager* GoldManager = nullptr;
};