#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class UE5GROUPPROJECT_API AProjectile : public AActor
{
    GENERATED_BODY()

public:
    AProjectile();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);

public:
    UPROPERTY(VisibleAnywhere)
    USphereComponent* Collision;

    UPROPERTY(VisibleAnywhere)
    UProjectileMovementComponent* Movement;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* ProjectileMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float Damage = 20.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float Speed = 1500.f;
};