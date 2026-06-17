#include "Weapon/Weapon.h"
#include "Character/FirstPersonCharacter.h"
#include "Net/UnrealNetwork.h"

AWeapon::AWeapon()
{
    bReplicates = true;
    CurrentAmmo = AmmoPerMagazine;
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AWeapon, CurrentAmmo);
    DOREPLIFETIME(AWeapon, OwnerCharacter);
}

void AWeapon::Fire(FVector FireStart, FVector FireDirection)
{
    if (!HasAuthority())
    {
        ServerFire(FireStart, FireDirection);
        return;
    }

    ServerFire_Implementation(FireStart, FireDirection);
}

void AWeapon::ServerFire_Implementation(FVector FireStart, FVector FireDirection)
{
    if (!HasAuthority() || !HasAmmo())
        return;

    CurrentAmmo--;
    LastFireEnd = FireStart + (FireDirection * Range);

    MulticastFireEffect(FireStart, LastFireEnd);
}

void AWeapon::MulticastFireEffect_Implementation(FVector FireStart, FVector FireEnd)
{
    UE_LOG(LogFirstPersonDemo, Warning, TEXT("Weapon fired from %s to %s"), *FireStart.ToString(), *FireEnd.ToString());
}

void AWeapon::EquipWeapon(AFirstPersonCharacter* NewOwner)
{
    if (!HasAuthority())
        return;

    OwnerCharacter = NewOwner;
    CurrentAmmo = AmmoPerMagazine;
}

void AWeapon::UnequipWeapon()
{
    OwnerCharacter = nullptr;
}

void AWeapon::Reload()
{
    if (!HasAuthority())
        return;

    CurrentAmmo = AmmoPerMagazine;
    UE_LOG(LogFirstPersonDemo, Warning, TEXT("Weapon reloaded"));
}
