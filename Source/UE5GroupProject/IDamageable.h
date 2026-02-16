#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDamageable.generated.h"

// Unreal's UObject interface wrapper
UINTERFACE(MinimalAPI)
class UDamageable : public UInterface
{
    GENERATED_BODY()
};

// Actual C++ interface
class UE5GROUPPROJECT_API IDamageable
{
    GENERATED_BODY()

public:
    // Any class implementing this interface must define ApplyDamage()
    virtual void ApplyDamage(float DamageAmount) = 0;
};