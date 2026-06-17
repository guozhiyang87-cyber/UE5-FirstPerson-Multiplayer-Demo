#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class AFirstPersonCharacter;

/**
 * Base Weapon Class
 * Can be extended for different weapon types
 */
AWeapon : public AActor
{
    GENERATED_BODY()

public:
    AWeapon();

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Weapon properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Damage = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float FireRate = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Range = 10000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 AmmoPerMagazine = 30;

    UPROPERTY(Replicated, BlueprintReadWrite, Category = "Weapon")
    int32 CurrentAmmo;

    UPROPERTY(Replicated, BlueprintReadWrite, Category = "Weapon")
    AFirstPersonCharacter* OwnerCharacter;

    // Fire
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void Fire(FVector FireStart, FVector FireDirection);

    UFUNCTION(Server, Reliable)
    void ServerFire(FVector FireStart, FVector FireDirection);

    UFUNCTION(NetMulticast, Unreliable)
    void MulticastFireEffect(FVector FireStart, FVector FireEnd);

    // Equip/Unequip
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void EquipWeapon(AFirstPersonCharacter* NewOwner);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void UnequipWeapon();

    // Ammo
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool HasAmmo() const { return CurrentAmmo > 0; }

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void Reload();

private:
    FVector LastFireEnd = FVector::ZeroVector;
};
