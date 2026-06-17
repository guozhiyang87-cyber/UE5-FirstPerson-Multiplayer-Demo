#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class AEnemyCharacter;
class AFirstPersonCharacter;

/**
 * AI Controller for Enemy Characters
 * Manages enemy behavior (patrol, chase, attack)
 */
AEnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    AEnemyAIController();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void OnPossess(APawn* InPawn) override;

    // AI States
    void PatrolBehavior();
    void ChaseBehavior(ACharacter* Target);
    void AttackBehavior(ACharacter* Target);

    // Helper functions
    ACharacter* FindClosestPlayer();
    float GetDistanceToPlayer(ACharacter* Player);

private:
    UPROPERTY()
    AEnemyCharacter* PossessedEnemy;

    ACharacter* CurrentTarget = nullptr;
    FVector PatrolLocation;
    float PatrolUpdateInterval = 5.0f;
    float PatrolUpdateTimer = 0.0f;

    // Behavior timers
    float BehaviorUpdateInterval = 0.5f;
    float BehaviorUpdateTimer = 0.0f;

    // Patrol point generation
    void GenerateRandomPatrolPoint();
};
