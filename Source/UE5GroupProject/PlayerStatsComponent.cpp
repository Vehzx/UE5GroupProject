#include "PlayerStatsComponent.h"

UPlayerStatsComponent::UPlayerStatsComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerStatsComponent::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    CurrentGold = StartingGold;

    UE_LOG(LogTemp, Warning, TEXT("PlayerStatsComponent initialized. Health: %f, Gold: %d"),
        CurrentHealth, CurrentGold);
}

bool UPlayerStatsComponent::SpendGold(int32 Amount)
{
    UE_LOG(LogTemp, Warning, TEXT("PlayerStatsComponent::SpendGold called. Amount: %d, CurrentGold BEFORE: %d"), Amount, CurrentGold);

    if (CurrentGold >= Amount)
    {
        CurrentGold -= Amount;
        UE_LOG(LogTemp, Warning, TEXT("Gold spent. CurrentGold AFTER: %d"), CurrentGold);

        OnGoldChanged.Broadcast(CurrentGold, -Amount);
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("PlayerStatsComponent::SpendGold FAILED. Not enough gold. CurrentGold: %d, Needed: %d"), CurrentGold, Amount);
    return false;
}

void UPlayerStatsComponent::AddGold(int32 Amount)
{
    UE_LOG(LogTemp, Warning, TEXT("PlayerStatsComponent::AddGold called. Amount: %d, CurrentGold BEFORE: %d"), Amount, CurrentGold);

    CurrentGold += Amount;

    UE_LOG(LogTemp, Warning, TEXT("PlayerStatsComponent::AddGold complete. CurrentGold AFTER: %d"), CurrentGold);

    OnGoldChanged.Broadcast(CurrentGold, Amount);
}

void UPlayerStatsComponent::ApplyDamageToPlayer(float Amount)
{
    UE_LOG(LogTemp, Warning, TEXT("PlayerStatsComponent::ApplyDamageToPlayer called. Damage: %f, CurrentHealth BEFORE: %f"), Amount, CurrentHealth);

    CurrentHealth -= Amount;

    UE_LOG(LogTemp, Warning, TEXT("PlayerStatsComponent::ApplyDamageToPlayer complete. CurrentHealth AFTER: %f"), CurrentHealth);

    OnHealthChanged.Broadcast(CurrentHealth, -Amount);

    if (CurrentHealth <= 0.f)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerStatsComponent: PLAYER DIED. Broadcasting OnPlayerDeath."));
        OnPlayerDeath.Broadcast();
    }
}