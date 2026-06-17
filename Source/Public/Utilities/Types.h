#pragma once

#include "CoreMinimal.h"

// Logging
UE_DEFINE_LOG_CATEGORY_STATIC(LogFirstPersonDemo, Log, All);

// Game state enums
UENUM(BlueprintType)
enum class EGameStateType : uint8
{
    WaitingForPlayers = 0 UMETA(DisplayName = "Waiting For Players"),
    InProgress = 1 UMETA(DisplayName = "In Progress"),
    GameOver = 2 UMETA(DisplayName = "Game Over")
};

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
    Alive = 0 UMETA(DisplayName = "Alive"),
    Dead = 1 UMETA(DisplayName = "Dead")
};

// Delegate for game events
DECLARE_MULTICAST_DELEGATE(FOnGameStateChanged);
DECLARE_MULTICAST_DELEGATE_ONE_PARAM(FOnPlayerDeath, class AFirstPersonCharacter*, Player);
DECLARE_MULTICAST_DELEGATE_ONE_PARAM(FOnEnemyDeath, class AEnemyCharacter*, Enemy);
DECLARE_MULTICAST_DELEGATE_ONE_PARAM(FOnPlayerScoreChanged, class AFirstPersonCharacter*, Player);
