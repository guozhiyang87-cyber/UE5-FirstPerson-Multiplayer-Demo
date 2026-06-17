#include "Game/MultiplayerPlayerController.h"
#include "Character/FirstPersonCharacter.h"
#include "Game/MultiplayerGameState.h"

void AMultiplayerPlayerController::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter = Cast<AFirstPersonCharacter>(GetPawn());

    if (HUDWidgetClass && IsLocalController())
    {
        HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
        if (HUDWidget)
        {
            HUDWidget->AddToViewport();
        }
    }
}

void AMultiplayerPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
}

void AMultiplayerPlayerController::ShowGameOver(bool bPlayerWon)
{
    if (IsLocalController())
    {
        if (bPlayerWon)
        {
            UE_LOG(LogFirstPersonDemo, Warning, TEXT("VICTORY! You are the last survivor!"));
        }
        else
        {
            UE_LOG(LogFirstPersonDemo, Warning, TEXT("DEFEAT! You have been eliminated!"));
        }
    }
}

void AMultiplayerPlayerController::UpdatePlayerStats()
{
    if (!PlayerCharacter)
        return;

    if (AMultiplayerGameState* GameState = GetWorld()->GetGameState<AMultiplayerGameState>())
    {
        UE_LOG(LogFirstPersonDemo, Warning, TEXT("Score: %d | Enemies Killed: %d"), 
            PlayerCharacter->PlayerStats.Score, 
            PlayerCharacter->PlayerStats.EnemiesKilled);
    }
}
