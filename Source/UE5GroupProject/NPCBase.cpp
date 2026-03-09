#include "NPCBase.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "PlayerStatsComponent.h"
#include "Kismet/GameplayStatics.h"

ANPCBase::ANPCBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create widget component for displaying health bar.
    HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
    HealthWidgetComponent->SetupAttachment(RootComponent);

    // Screen space ensures the widget always faces the camera.
    HealthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

    // Set size for the health bar widget (this will override the blueprint editor! -Luke)
    HealthWidgetComponent->SetDrawSize(FVector2D(200.f, 20.f));
}


void ANPCBase::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;

    // Temporary damage call for testing. 
    // Remove once proper tower damage is implemented.
    ApplyDamage(20.f);
}

void ANPCBase::OnDeath_Implementation()
{
    // Blueprint only hook for death visuals.
    // Actual death logic is handled in ApplyDamage().
}

void ANPCBase::HandleReachedGoal()
{
    // Get PlayerStats from PlayerController
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        if (UPlayerStatsComponent* Stats = PC->FindComponentByClass<UPlayerStatsComponent>())
        {
            Stats->ApplyDamageToPlayer(10.f); // Damage amount per escaped enemy
        }
    }

    Destroy(); // Remove NPC from the world
}

void ANPCBase::ApplyDamage(float DamageAmount)
{
    UE_LOG(LogTemp, Warning, TEXT("ANPCBase::ApplyDamage called on %s, DamageAmount: %f (CurrentHealth: %f)"),
        *GetName(), DamageAmount, CurrentHealth);

    CurrentHealth -= DamageAmount;

    if (UEnemyHealthBarWidget* HealthWidget =
        Cast<UEnemyHealthBarWidget>(HealthWidgetComponent->GetUserWidgetObject()))
    {
        HealthWidget->SetHealth(CurrentHealth, MaxHealth);
    }

    if (CurrentHealth <= 0.f)
    {
        if (AGoldManager* GM = GoldManager)
        {
            GM->AddGold(10); // temporary, can add tower specific rewards later
        }

        UE_LOG(LogTemp, Warning, TEXT("%s died"), *GetName());
        OnDeath();
        Destroy();
    }
}