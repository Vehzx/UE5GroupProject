#include "PlayerStatsComponent.h"

UPlayerStatsComponent::UPlayerStatsComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UPlayerStatsComponent::SpendGold(int32 Amount)
{
    if (CurrentGold >= Amount)
    {
        CurrentGold -= Amount;
        OnGoldChanged.Broadcast(CurrentGold, -Amount);
        return true;
    }
    return false;
}

void UPlayerStatsComponent::AddGold(int32 Amount)
{
    CurrentGold += Amount;
    OnGoldChanged.Broadcast(CurrentGold, Amount);
}

void UPlayerStatsComponent::ApplyDamageToPlayer(float Amount)
{
    CurrentHealth -= Amount;
    OnHealthChanged.Broadcast(CurrentHealth, -Amount);

    if (CurrentHealth <= 0.f)
    {
        OnPlayerDeath.Broadcast();
    }
}