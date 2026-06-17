#include "Game/MultiplayerGameState.h"
#include "Character/FirstPersonCharacter.h"
#include "Net/UnrealNetwork.h"

AMultiplayerGameState::AMultiplayerGameState()
{
    bReplicates = true;
}

void AMultiplayerGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMultiplayerGameState, Players);
    DOREPLIFETIME(AMultiplayerGameState, AlivePlayerCount);
    DOREPLIFETIME(AMultiplayerGameState, TotalEnemiesDefeated);
    DOREPLIFETIME(AMultiplayerGameState, CurrentWave);
}

void AMultiplayerGameState::RegisterPlayer(AFirstPersonCharacter* Player)
{
    if (!IsLocallyControlled())
        return;

    ServerRegisterPlayer(Player);
}

void AMultiplayerGameState::ServerRegisterPlayer_Implementation(AFirstPersonCharacter* Player)
{
    if (!HasAuthority() || !Player)
        return;

    FPlayerInfo NewPlayerInfo;
    NewPlayerInfo.PlayerCharacter = Player;
    NewPlayerInfo.PlayerIndex = Players.Num();
    NewPlayerInfo.bIsAlive = true;

    Players.Add(NewPlayerInfo);
    AlivePlayerCount = Players.Num();

    UE_LOG(LogFirstPersonDemo, Warning, TEXT("Player registered. Total players: %d"), Players.Num());
}

void AMultiplayerGameState::OnPlayerDeath(AFirstPersonCharacter* Player)
{
    ServerOnPlayerDeath(Player);
}

void AMultiplayerGameState::ServerOnPlayerDeath_Implementation(AFirstPersonCharacter* Player)
{
    if (!HasAuthority())
        return;

    for (FPlayerInfo& PlayerInfo : Players)
    {
        if (PlayerInfo.PlayerCharacter == Player)
        {
            PlayerInfo.bIsAlive = false;
            AlivePlayerCount--;
            break;
        }
    }
}

void AMultiplayerGameState::OnPlayerAddScore(AFirstPersonCharacter* Player, int32 Score)
{
    ServerOnPlayerAddScore(Player, Score);
}

void AMultiplayerGameState::ServerOnPlayerAddScore_Implementation(AFirstPersonCharacter* Player, int32 Score)
{
    if (!HasAuthority())
        return;

    FPlayerInfo* PlayerInfo = FindPlayerInfo(Player);
    if (PlayerInfo)
    {
        PlayerInfo->Score += Score;
    }
}

FPlayerInfo* AMultiplayerGameState::FindPlayerInfo(AFirstPersonCharacter* Player)
{
    for (FPlayerInfo& PlayerInfo : Players)
    {
        if (PlayerInfo.PlayerCharacter == Player)
        {
            return &PlayerInfo;
        }
    }
    return nullptr;
}

AFirstPersonCharacter* AMultiplayerGameState::GetLastAliveSurvivor()
{
    for (const FPlayerInfo& PlayerInfo : Players)
    {
        if (PlayerInfo.bIsAlive)
        {
            return PlayerInfo.PlayerCharacter;
        }
    }
    return nullptr;
}
