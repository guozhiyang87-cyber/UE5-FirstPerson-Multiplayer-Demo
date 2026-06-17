#include "Character/FirstPersonCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Weapon/Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Game/MultiplayerGameState.h"
#include "DrawDebugHelpers.h"

AFirstPersonCharacter::AFirstPersonCharacter()
{
    // Don't rotate character with camera
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->MaxWalkSpeed = 800.0f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
    GetCharacterMovement()->MaxWalkSpeedCrouched = 400.0f;

    // Create first person camera
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(RootComponent);
    FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
    FirstPersonCamera->bUsePawnControlRotation = true;

    // Enable replication
    bReplicates = true;
    bReplicateMovement = true;

    // Initialize health
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    PlayerStats.bIsAlive = true;
}

void AFirstPersonCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocallyControlled())
    {
        // Add Input Mapping Context
        if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }

    // Register with game state
    if (AMultiplayerGameState* GameState = GetWorld()->GetGameState<AMultiplayerGameState>())
    {
        GameState->ServerRegisterPlayer(this);
    }

    // Equip weapon
    if (HasAuthority())
    {
        EquipWeapon();
    }
}

void AFirstPersonCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AFirstPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = dynamic_cast<UEnhancedInputComponent*>(PlayerInputComponent))
    {
        // Moving
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacter::Move);
        }

        // Looking
        if (LookAction)
        {
            EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacter::Look);
        }

        // Fire
        if (FireAction)
        {
            EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AFirstPersonCharacter::StartFire);
            EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AFirstPersonCharacter::StopFire);
        }
    }
}

void AFirstPersonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFirstPersonCharacter, CurrentHealth);
    DOREPLIFETIME(AFirstPersonCharacter, PlayerStats);
}

void AFirstPersonCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // Move forward/backward
        AddMovementInput(GetActorForwardVector(), MovementVector.Y);

        // Move left/right
        AddMovementInput(GetActorRightVector(), MovementVector.X);
    }
}

void AFirstPersonCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // Look up/down
        AddControllerPitchInput(LookAxisVector.Y);

        // Look left/right
        AddControllerYawInput(LookAxisVector.X);
    }
}

void AFirstPersonCharacter::StartFire()
{
    if (!IsLocallyControlled() || !PlayerStats.bIsAlive)
        return;

    Fire();
}

void AFirstPersonCharacter::StopFire()
{
    // Could be used for automatic fire later
}

void AFirstPersonCharacter::Fire()
{
    if (!bCanFire || !CurrentWeapon || !PlayerStats.bIsAlive)
        return;

    bCanFire = false;
    GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &AFirstPersonCharacter::ResetFireRate, FireRateDelay, false);

    FVector CameraLocation = FirstPersonCamera->GetComponentLocation();
    FVector CameraForward = FirstPersonCamera->GetForwardVector();

    CurrentWeapon->Fire(CameraLocation, CameraForward);
}

void AFirstPersonCharacter::ServerFire_Implementation(FVector FireLocation, FVector FireDirection)
{
    if (!HasAuthority())
        return;

    FHitResult HitResult;
    FVector TraceEnd = FireLocation + (FireDirection * 10000.0f);
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(HitResult, FireLocation, TraceEnd, ECC_Pawn, QueryParams))
    {
        if (AEnemyCharacter* HitEnemy = Cast<AEnemyCharacter>(HitResult.GetActor()))
        {
            HitEnemy->ServerTakeDamage(25.0f);
            AddScore(10);
        }
    }

    MulticastPlayFireEffect(FireLocation);
}

void AFirstPersonCharacter::MulticastPlayFireEffect_Implementation(FVector FireLocation)
{
    // Play firing effects (sound, animation, particles)
    UE_LOG(LogFirstPersonDemo, Warning, TEXT("Player fired weapon"));
}

void AFirstPersonCharacter::ResetFireRate()
{
    bCanFire = true;
}

void AFirstPersonCharacter::OnRep_Health()
{
    if (CurrentHealth <= 0.0f && PlayerStats.bIsAlive)
    {
        Die();
    }
}

void AFirstPersonCharacter::OnRep_PlayerStats()
{
    // Update UI or other game logic based on player stats changes
}

void AFirstPersonCharacter::TakeDamage(float DamageAmount)
{
    if (HasAuthority())
    {
        ServerTakeDamage(DamageAmount);
    }
}

void AFirstPersonCharacter::ServerTakeDamage_Implementation(float DamageAmount)
{
    if (!HasAuthority())
        return;

    CurrentHealth -= DamageAmount;
    CurrentHealth = FMath::Max(0.0f, CurrentHealth);

    MulticastTakeDamage(DamageAmount);

    if (CurrentHealth <= 0.0f)
    {
        ServerDie();
    }
}

void AFirstPersonCharacter::MulticastTakeDamage_Implementation(float DamageAmount)
{
    // Play damage effects
    UE_LOG(LogFirstPersonDemo, Warning, TEXT("%s took damage"), *GetName());
}

void AFirstPersonCharacter::Die()
{
    ServerDie();
}

void AFirstPersonCharacter::ServerDie_Implementation()
{
    if (!HasAuthority())
        return;

    if (!PlayerStats.bIsAlive)
        return;

    PlayerStats.bIsAlive = false;
    MulticastDie();

    // Disable character movement and collision
    GetCharacterMovement()->DisableMovement();
    GetCapsuleComponent()->SetCollisionEnabled(ECC_NoCollision);

    // Notify game state
    if (AMultiplayerGameState* GameState = GetWorld()->GetGameState<AMultiplayerGameState>())
    {
        GameState->ServerOnPlayerDeath(this);
    }
}

void AFirstPersonCharacter::MulticastDie_Implementation()
{
    // Play death effects, animation, etc.
    UE_LOG(LogFirstPersonDemo, Warning, TEXT("%s has died!"), *GetName());
}

void AFirstPersonCharacter::AddScore(int32 Points)
{
    if (!HasAuthority())
        return;

    PlayerStats.Score += Points;
    PlayerStats.EnemiesKilled += 1;
}

void AFirstPersonCharacter::EquipWeapon()
{
    if (!HasAuthority())
        return;

    if (WeaponClass)
    {
        CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
        if (CurrentWeapon)
        {
            CurrentWeapon->EquipWeapon(this);
        }
    }
}
