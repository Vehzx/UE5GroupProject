#include "GoldManager.h"

AGoldManager::AGoldManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

bool AGoldManager::SpendGold(int32 Amount)
{
    if (CurrentGold >= Amount)
    {
        CurrentGold -= Amount;
        return true;
    }
    return false;
}

void AGoldManager::AddGold(int32 Amount)
{
    CurrentGold += Amount;
}