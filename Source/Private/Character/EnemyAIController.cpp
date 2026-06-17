#include "Character/EnemyAIController.h"
#include "Character/FirstPersonCharacter.h"
#include "NavigationSystem.h"

AEnemyAIController::AEnemyAIController()
{
    bWantsTick = true;
}

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();
    GenerateRandomPatrolPoint();
}

void AEnemyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!PossessedEnemy)
        return;

    BehaviorUpdateTimer += DeltaTime;
    if (BehaviorUpdateTimer >= BehaviorUpdateInterval)
    {
        BehaviorUpdateTimer = 0.0f;

        ACharacter* ClosestPlayer = FindClosestPlayer();

        if (ClosestPlayer)
        {
            float DistanceToPlayer = GetDistanceToPlayer(ClosestPlayer);

            if (DistanceToPlayer < PossessedEnemy->AttackRadius)
            {
                AttackBehavior(ClosestPlayer);
            }
            else if (DistanceToPlayer < PossessedEnemy->DetectionRadius)
            {
                ChaseBehavior(ClosestPlayer);
            }
            else
            {
                PatrolBehavior();
            }
        }
        else
        {
            PatrolBehavior();
        }
    }

    PatrolUpdateTimer += DeltaTime;
    if (PatrolUpdateTimer >= PatrolUpdateInterval)
    {
        PatrolUpdateTimer = 0.0f;
        if (!CurrentTarget)
        {
            GenerateRandomPatrolPoint();
        }
    }
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    PossessedEnemy = Cast<AEnemyCharacter>(InPawn);
}

void AEnemyAIController::PatrolBehavior()
{
    if (!PossessedEnemy)
        return;

    PossessedEnemy->CurrentState = EEnemyState::Patrol;
    MoveToLocation(PatrolLocation, 100.0f);
}

void AEnemyAIController::ChaseBehavior(ACharacter* Target)
{
    if (!PossessedEnemy || !Target)
        return;

    PossessedEnemy->CurrentState = EEnemyState::Chase;
    MoveToActor(Target, PossessedEnemy->AttackRadius * 0.5f);
    CurrentTarget = Target;
}

void AEnemyAIController::AttackBehavior(ACharacter* Target)
{
    if (!PossessedEnemy || !Target)
        return;

    PossessedEnemy->CurrentState = EEnemyState::Attack;
    StopMovement();

    float TimeSinceLastAttack = GetWorld()->GetTimeSeconds() - PossessedEnemy->LastAttackTime;
    if (TimeSinceLastAttack >= PossessedEnemy->AttackCooldown)
    {
        PossessedEnemy->AttackTarget(Target);
        PossessedEnemy->LastAttackTime = GetWorld()->GetTimeSeconds();
    }
}

ACharacter* AEnemyAIController::FindClosestPlayer()
{
    if (!PossessedEnemy)
        return nullptr;

    float ClosestDistance = FLT_MAX;
    ACharacter* ClosestPlayer = nullptr;

    for (TActorIterator<AFirstPersonCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        if (ActorItr->IsAlive())
        {
            float Distance = FVector::Dist(PossessedEnemy->GetActorLocation(), ActorItr->GetActorLocation());
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                ClosestPlayer = *ActorItr;
            }
        }
    }

    return ClosestPlayer;
}

float AEnemyAIController::GetDistanceToPlayer(ACharacter* Player)
{
    if (!PossessedEnemy || !Player)
        return FLT_MAX;

    return FVector::Dist(PossessedEnemy->GetActorLocation(), Player->GetActorLocation());
}

void AEnemyAIController::GenerateRandomPatrolPoint()
{
    if (!PossessedEnemy)
        return;

    FVector RandomDirection = FMath::RandPointInCircle(500.0f);
    PatrolLocation = PossessedEnemy->GetActorLocation() + FVector(RandomDirection.X, RandomDirection.Y, 0.0f);
}
