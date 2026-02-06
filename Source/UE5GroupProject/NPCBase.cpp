#include "NPCBase.h"

ANPCBase::ANPCBase()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ANPCBase::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
}

void ANPCBase::ApplyDamage(float DamageAmount)
{
    CurrentHealth -= DamageAmount;

    if (CurrentHealth <= 0.f)
    {
        OnDeath();
        Destroy();
    }
}