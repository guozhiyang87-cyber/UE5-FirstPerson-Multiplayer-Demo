#include "Game/MultiplayerGameMode.h"
#include "Game/MultiplayerGameState.h"
#include "Character/EnemyCharacter.h"
#include "Net/UnrealNetwork.h"

AMultiplayerGameMode::AMultiplayerGameMode()
{
    bUseStatelessNetworkReplication = false;
    bReplicates = true;
}

void AMultiplayerGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        GameStateInstance = GetGameState<AMultiplayerGameState>();

        // Generate spawn points
        for (int32 i = 0; i < 8; i++)
        {
            FVector SpawnPoint = FVector(FMath::RandRange(-2000, 2000), FMath::RandRange(-2000, 2000), 100);
            EnemySpawnPoints.Add(SpawnPoint);
        }

        // Start game after delay
        GetWorld()->GetTimerManager().SetTimer(WaveSpawnTimerHandle, this, &AMultiplayerGameMode::SpawnEnemyWave, WaveInterval, true, 5.0f);
    }
}

void AMultiplayerGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (HasAuthority())
    {
        CheckGameOver();
    }
}

void AMultiplayerGameMode::StartGame()
{
    if (!HasAuthority())
        return;

    CurrentGameState = EGameState::InProgress;
    bGameStarted = true;
    CurrentWave = 1;
}

void AMultiplayerGameMode::EndGame(bool bPlayerWon)
{
    if (!HasAuthority())
        return;

    CurrentGameState = EGameState::GameOver;
    GetWorld()->GetTimerManager().ClearTimer(WaveSpawnTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(EnemySpawnTimerHandle);
}

void AMultiplayerGameMode::CheckGameOver()
{
    if (CurrentGameState != EGameState::InProgress)
        return;

    if (!GameStateInstance)
        return;

    // Check if all players are dead
    if (GameStateInstance->AlivePlayerCount <= 1)
    {
        EndGame(GameStateInstance->AlivePlayerCount == 1);
    }
}

void AMultiplayerGameMode::ServerRespawnPlayer_Implementation(AFirstPersonCharacter* DeadPlayer)
{
    if (!HasAuthority() || !DeadPlayer)
        return;
    
    FTransform SpawnTransform = GetRandomSpawnPoint();
    DeadPlayer->SetActorTransform(SpawnTransform);
    DeadPlayer->ServerRevive();
}

void AMultiplayerGameMode::SpawnEnemyWave()
{
    if (!HasAuthority())
        return;

    ServerSpawnEnemyWave();
}

void AMultiplayerGameMode::ServerSpawnEnemyWave_Implementation()
{
    if (!HasAuthority() || !EnemyClass)
        return;

    CurrentWave++;
    int32 EnemiesToSpawn = InitialEnemyCount + CurrentWave - 1;
    EnemiesSpawnedThisWave = 0;

    // Spawn enemies over time
    for (int32 i = 0; i < EnemiesToSpawn; i++)
    {
        FVector SpawnLocation = EnemySpawnPoints[FMath::RandRange(0, EnemySpawnPoints.Num() - 1)];
        FRotator SpawnRotation = FRotator::ZeroRotator;

        AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnLocation, SpawnRotation);
        if (SpawnedEnemy)
        {
            ActiveEnemyCount++;
        }
    }

    UE_LOG(LogFirstPersonDemo, Warning, TEXT("Wave %d started with %d enemies"), CurrentWave, EnemiesToSpawn);
}

void AMultiplayerGameMode::OnEnemyDeath()
{
    ServerOnEnemyDeath();
}

void AMultiplayerGameMode::ServerOnEnemyDeath_Implementation()
{
    if (!HasAuthority())
        return;

    ActiveEnemyCount--;
    if (GameStateInstance)
    {
        GameStateInstance->TotalEnemiesDefeated++;
    }
}
