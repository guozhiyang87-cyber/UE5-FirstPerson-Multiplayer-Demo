#include "Character/EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Character/EnemyAIController.h"
#include "Game/MultiplayerGameMode.h"

AEnemyCharacter::AEnemyCharacter()
{
    // Don't rotate character with camera
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;

    // Enable replication
    bReplicates = true;
    bReplicateMovement = true;

    CurrentHealth = MaxHealth;
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;

    if (HasAuthority())
    {
        // Spawn AI controller
        AEnemyAIController* AIController = GetWorld()->SpawnActor<AEnemyAIController>();
        if (AIController)
        {
            AIController->Possess(this);
            EnemyAIController = AIController;
        }
    }
}

void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AEnemyCharacter, CurrentHealth);
    DOREPLIFETIME(AEnemyCharacter, CurrentState);
}

void AEnemyCharacter::OnRep_CurrentHealth()
{
    if (CurrentHealth <= 0.0f && !bIsDead)
    {
        Die();
    }
}

void AEnemyCharacter::OnRep_CurrentState()
{
    // Handle visual state changes
}

void AEnemyCharacter::TakeDamage(float DamageAmount)
{
    if (HasAuthority())
    {
        ServerTakeDamage(DamageAmount);
    }
}

void AEnemyCharacter::ServerTakeDamage_Implementation(float DamageAmount)
{
    if (!HasAuthority() || bIsDead)
        return;

    CurrentHealth -= DamageAmount;
    CurrentHealth = FMath::Max(0.0f, CurrentHealth);

    MulticastTakeDamage(DamageAmount);

    if (CurrentHealth <= 0.0f)
    {
        ServerDie();
    }
}

void AEnemyCharacter::MulticastTakeDamage_Implementation(float DamageAmount)
{
    // Play damage effects
    UE_LOG(LogFirstPersonDemo, Warning, TEXT("Enemy took damage: %f"), DamageAmount);
}

void AEnemyCharacter::Die()
{
    ServerDie();
}

void AEnemyCharacter::ServerDie_Implementation()
{
    if (!HasAuthority() || bIsDead)
        return;

    bIsDead = true;
    CurrentState = EEnemyState::Dead;
    MulticastDie();

    // Disable movement and collision
    GetCharacterMovement()->DisableMovement();
    GetCapsuleComponent()->SetCollisionEnabled(ECC_NoCollision);

    // Notify game mode
    if (AMultiplayerGameMode* GameMode = GetWorld()->GetAuthGameMode<AMultiplayerGameMode>())
    {
        GameMode->ServerOnEnemyDeath();
    }
}

void AEnemyCharacter::MulticastDie_Implementation()
{
    UE_LOG(LogFirstPersonDemo, Warning, TEXT("Enemy died!"));
}

ACharacter* AEnemyCharacter::FindClosestPlayer()
{
    float ClosestDistance = FLT_MAX;
    ACharacter* ClosestPlayer = nullptr;

    for (TActorIterator<AFirstPersonCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        if (ActorItr->IsAlive())
        {
            float Distance = FVector::Dist(GetActorLocation(), ActorItr->GetActorLocation());
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                ClosestPlayer = *ActorItr;
            }
        }
    }

    return ClosestPlayer;
}

void AEnemyCharacter::AttackTarget(ACharacter* Target)
{
    if (HasAuthority())
    {
        ServerAttackTarget(Target);
    }
}

void AEnemyCharacter::ServerAttackTarget_Implementation(ACharacter* Target)
{
    if (!HasAuthority() || !Target)
        return;

    if (AFirstPersonCharacter* PlayerTarget = Cast<AFirstPersonCharacter>(Target))
    {
        PlayerTarget->ServerTakeDamage(AttackDamage);
        MulticastAttackEffect();
    }
}

void AEnemyCharacter::MulticastAttackEffect_Implementation()
{
    UE_LOG(LogFirstPersonDemo, Warning, TEXT("Enemy attacked!"));
}
