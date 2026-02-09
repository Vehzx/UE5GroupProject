#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPCBase.generated.h"

UCLASS()
class UE5GROUPPROJECT_API ANPCBase : public ACharacter
{
    GENERATED_BODY()

public:
    ANPCBase();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Stats")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC|Stats")
    float CurrentHealth;

    UFUNCTION(BlueprintCallable, Category = "NPC|Combat")
    void ApplyDamage(float DamageAmount);

    UFUNCTION(BlueprintImplementableEvent, Category = "NPC|Combat")
    void OnDeath();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    class UWidgetComponent* HealthWidgetComponent;
};