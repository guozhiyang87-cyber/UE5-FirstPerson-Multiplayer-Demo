#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MultiplayerPlayerController.generated.h"

class AFirstPersonCharacter;
class AMultiplayerGameState;

/**
 * Multiplayer Player Controller
 * Handles player input and game UI
 */
AMultiplayerPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // Game UI
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UUserWidget> HUDWidgetClass;

    UPROPERTY(BlueprintReadWrite, Category = "UI")
    class UUserWidget* HUDWidget;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowGameOver(bool bPlayerWon);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdatePlayerStats();

private:
    UPROPERTY()
    AFirstPersonCharacter* PlayerCharacter;
};
