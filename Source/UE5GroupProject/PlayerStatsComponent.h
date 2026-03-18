#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGoldChanged, int32, NewGold, int32, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, Delta);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE5GROUPPROJECT_API UPlayerStatsComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPlayerStatsComponent();

    virtual void BeginPlay() override;

    // --- Player Health ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats")
    float CurrentHealth;

    // --- Player Gold ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
    int32 StartingGold = 200;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats")
    int32 CurrentGold;

    // --- Run Statistics ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats")
    int32 GoldEarned = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats")
    int32 GoldSpent = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats")
    int32 EnemiesKilled = 0;

    // --- Events for UI ---
    UPROPERTY(BlueprintAssignable)
    FOnPlayerDeath OnPlayerDeath;

    UPROPERTY(BlueprintAssignable)
    FOnGoldChanged OnGoldChanged;

    UPROPERTY(BlueprintAssignable)
    FOnHealthChanged OnHealthChanged;

    // --- Gold Functions ---
    bool SpendGold(int32 Amount);
    void AddGold(int32 Amount);

    // --- Health Functions ---
    void ApplyDamageToPlayer(float Amount);
};