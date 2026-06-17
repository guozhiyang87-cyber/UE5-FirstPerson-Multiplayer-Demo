#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class AEnemyAIController;
class AFirstPersonCharacter;

/**
 * Enemy Character for AI controlled enemies
 * Handles enemy health, damage, and death
 */
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    Patrol = 0 UMETA(DisplayName = "Patrol"),
    Chase = 1 UMETA(DisplayName = "Chase"),
    Attack = 2 UMETA(DisplayName = "Attack"),
    Dead = 3 UMETA(DisplayName = "Dead")
};

AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Health system
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float MaxHealth = 30.0f;

    UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, BlueprintReadWrite, Category = "Health")
    float CurrentHealth;

    UPROPERTY(Replicated, BlueprintReadWrite, Category = "State")
    EEnemyState CurrentState = EEnemyState::Patrol;

    // AI
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float DetectionRadius = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackRadius = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackDamage = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackCooldown = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float MovementSpeed = 600.0f;

    UPROPERTY(BlueprintReadWrite, Category = "AI")
    float LastAttackTime = 0.0f;

    // Damage
    UFUNCTION(BlueprintCallable, Category = "Health")
    void TakeDamage(float DamageAmount);

    UFUNCTION(Server, Reliable)
    void ServerTakeDamage(float DamageAmount);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastTakeDamage(float DamageAmount);

    // Death
    UFUNCTION(BlueprintCallable, Category = "Health")
    void Die();

    UFUNCTION(Server, Reliable)
    void ServerDie();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastDie();

    // Replication
    UFUNCTION()
    void OnRep_CurrentHealth();

    UFUNCTION()
    void OnRep_CurrentState();

    // AI Helper functions
    UFUNCTION(BlueprintCallable, Category = "AI")
    ACharacter* FindClosestPlayer();

    UFUNCTION(BlueprintCallable, Category = "AI")
    void AttackTarget(ACharacter* Target);

    UFUNCTION(Server, Reliable)
    void ServerAttackTarget(ACharacter* Target);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastAttackEffect();

    // Get AI Controller
    FORCEINLINE AEnemyAIController* GetEnemyAIController() const { return EnemyAIController; }

private:
    AEnemyAIController* EnemyAIController;
    bool bIsDead = false;
};
