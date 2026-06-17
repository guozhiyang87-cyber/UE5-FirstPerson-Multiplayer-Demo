#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MultiplayerGameState.generated.h"

class AFirstPersonCharacter;

USTRUCT(BlueprintType)
struct FPlayerInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    AFirstPersonCharacter* PlayerCharacter = nullptr;

    UPROPERTY(BlueprintReadWrite)
    int32 Score = 0;

    UPROPERTY(BlueprintReadWrite)
    int32 EnemiesKilled = 0;

    UPROPERTY(BlueprintReadWrite)
    bool bIsAlive = true;

    UPROPERTY(BlueprintReadWrite)
    int32 PlayerIndex = -1;
};

/**
 * Multiplayer Game State
 * Maintains global game state and player information
 */
AMultiplayerGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    AMultiplayerGameState();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Player management
    UPROPERTY(Replicated, BlueprintReadWrite, Category = "Players")
    TArray<FPlayerInfo> Players;

    UPROPERTY(Replicated, BlueprintReadWrite, Category = "Game")
    int32 AlivePlayerCount = 0;

    UFUNCTION(BlueprintCallable, Category = "Players")
    void RegisterPlayer(AFirstPersonCharacter* Player);

    UFUNCTION(Server, Reliable)
    void ServerRegisterPlayer(AFirstPersonCharacter* Player);

    UFUNCTION(BlueprintCallable, Category = "Players")
    void OnPlayerDeath(AFirstPersonCharacter* Player);

    UFUNCTION(Server, Reliable)
    void ServerOnPlayerDeath(AFirstPersonCharacter* Player);

    UFUNCTION(BlueprintCallable, Category = "Players")
    void OnPlayerAddScore(AFirstPersonCharacter* Player, int32 Score);

    UFUNCTION(Server, Reliable)
    void ServerOnPlayerAddScore(AFirstPersonCharacter* Player, int32 Score);

    // Get player info
    UFUNCTION(BlueprintCallable, Category = "Players")
    FPlayerInfo* FindPlayerInfo(AFirstPersonCharacter* Player);

    UFUNCTION(BlueprintCallable, Category = "Players")
    AFirstPersonCharacter* GetLastAliveSurvivor();

    // Game info
    UPROPERTY(Replicated, BlueprintReadWrite, Category = "Game")
    int32 TotalEnemiesDefeated = 0;

    UPROPERTY(Replicated, BlueprintReadWrite, Category = "Game")
    int32 CurrentWave = 0;
};
