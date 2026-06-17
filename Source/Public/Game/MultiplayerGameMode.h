#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiplayerGameMode.generated.h"

class AEnemyCharacter;
class AMultiplayerGameState;

UENUM(BlueprintType)
enum class EGameState : uint8
{
    WaitingForPlayers = 0 UMETA(DisplayName = "Waiting For Players"),
    InProgress = 1 UMETA(DisplayName = "In Progress"),
    GameOver = 2 UMETA(DisplayName = "Game Over")
};

/**
 * Multiplayer Game Mode
 * Manages game flow, enemy spawning, and win conditions
 */
AMultiplayerGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AMultiplayerGameMode();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Game settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
    int32 MaxPlayers = 8;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
    int32 InitialEnemyCount = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
    float EnemySpawnInterval = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
    float WaveInterval = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
    TSubclassOf<AEnemyCharacter> EnemyClass;

    // Spawn points
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
    TArray<FVector> EnemySpawnPoints;

    // Game state
    UPROPERTY(BlueprintReadWrite, Category = "Game")
    EGameState CurrentGameState = EGameState::WaitingForPlayers;

    UPROPERTY(BlueprintReadWrite, Category = "Game")
    int32 CurrentWave = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Game")
    int32 ActiveEnemyCount = 0;

    // Game flow
    UFUNCTION(BlueprintCallable, Category = "Game")
    void StartGame();

    UFUNCTION(BlueprintCallable, Category = "Game")
    void EndGame(bool bPlayerWon);

    UFUNCTION(BlueprintCallable, Category = "Game")
    void CheckGameOver();

    // Enemy management
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void SpawnEnemyWave();

    UFUNCTION(Server, Reliable)
    void ServerSpawnEnemyWave();

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void OnEnemyDeath();

    UFUNCTION(Server, Reliable)
    void ServerOnEnemyDeath();

    // Get game state
    FORCEINLINE AMultiplayerGameState* GetMultiplayerGameState() const { return GameStateInstance; }

    UFUNCTION(BlueprintCallable, Category = "Player")
    void RespawnPlayer(AFirstPersonCharacter* DeadPlayer);
    
    UFUNCTION(Server, Reliable)
    void ServerRespawnPlayer(AFirstPersonCharacter* DeadPlayer);

private:
    AMultiplayerGameState* GameStateInstance;

    // Timers
    FTimerHandle WaveSpawnTimerHandle;
    FTimerHandle EnemySpawnTimerHandle;
    FTimerHandle GameCheckTimerHandle;

    int32 EnemiesSpawnedThisWave = 0;
    bool bGameStarted = false;
};
