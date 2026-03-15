#include "GoldManager.h"

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
}