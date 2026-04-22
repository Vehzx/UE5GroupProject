#include "GoldManager.h"
#include "PlayerStatsComponent.h"
#include "Kismet/GameplayStatics.h"

AGoldManager::AGoldManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

bool AGoldManager::SpendGold(int32 Amount)
{
    UE_LOG(LogTemp, Warning, TEXT("GoldManager::SpendGold called. Amount: %d, CurrentGold BEFORE: %d"), Amount, CurrentGold);

    if (CurrentGold >= Amount)
    {
        CurrentGold -= Amount;

        UE_LOG(LogTemp, Warning, TEXT("GoldManager::SpendGold success. CurrentGold AFTER: %d"), CurrentGold);

        if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
        {
            if (UPlayerStatsComponent* Stats = PC->FindComponentByClass<UPlayerStatsComponent>())
            {
                Stats->SpendGold(Amount);   // negative = spending
            }
        }

        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("GoldManager::SpendGold FAILED. Not enough gold. CurrentGold: %d, Needed: %d"), CurrentGold, Amount);
    return false;
}

void AGoldManager::AddGold(int32 Amount)
{
    UE_LOG(LogTemp, Warning, TEXT("GoldManager::AddGold called. Amount: %d, CurrentGold BEFORE: %d"), Amount, CurrentGold);

    CurrentGold += Amount;

    UE_LOG(LogTemp, Warning, TEXT("GoldManager::AddGold complete. CurrentGold AFTER: %d"), CurrentGold);

    // Forward the gold from the manager to PlayerStatsComponent
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        if (UPlayerStatsComponent* Stats = PC->FindComponentByClass<UPlayerStatsComponent>())
        {
            Stats->AddGold(Amount);
        }
    }
}