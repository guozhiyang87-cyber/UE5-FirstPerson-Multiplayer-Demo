#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FirstPersonCharacter.generated.h"

class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AWeapon;
class AEnemyCharacter;

/**
 * First Person Player Character
 * Handles player movement, combat, and networking
 */
USTRUCT(BlueprintType)
struct FPlayerStats
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    int32 Score = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    int32 EnemiesKilled = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    bool bIsAlive = true;
};

AFIRSTPersonCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AFirstPersonCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Replication
    virtual void OnRep_Health();
    UFUNCTION()
    void OnRep_PlayerStats();

    // Health system
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float MaxHealth;

    UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadWrite, Category = "Health")
    float CurrentHealth;

    UFUNCTION(BlueprintCallable, Category = "Health")
    void TakeDamage(float DamageAmount);

    UFUNCTION(Server, Reliable, BlueprintCallable)
    void ServerTakeDamage(float DamageAmount);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastTakeDamage(float DamageAmount);

    // Combat
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Fire();

    UFUNCTION(Server, Reliable)
    void ServerFire(FVector FireLocation, FVector FireDirection);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayFireEffect(FVector FireLocation);

    // Death
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Die();

    UFUNCTION(Server, Reliable)
    void ServerDie();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastDie();

    // Player Stats
    UPROPERTY(ReplicatedUsing = OnRep_PlayerStats, BlueprintReadWrite, Category = "Stats")
    FPlayerStats PlayerStats;

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void AddScore(int32 Points);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    bool IsAlive() const { return PlayerStats.bIsAlive; }

    // Weapon system
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<AWeapon> WeaponClass;

    UPROPERTY(BlueprintReadWrite, Category = "Weapon")
    AWeapon* CurrentWeapon;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void EquipWeapon();

    // Camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FirstPersonCamera;

private:
    // Enhanced Input System
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputAction* FireAction;

    // Input callbacks
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartFire();
    void StopFire();

    // Fire timer
    FTimerHandle FireRateTimerHandle;
    float FireRateDelay = 0.1f;
    bool bCanFire = true;

    void ResetFireRate();
};
