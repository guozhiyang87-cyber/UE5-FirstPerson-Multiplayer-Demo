#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FirstPersonHUD.generated.h"

UCLASS()
class AFirstPersonHUD : public AHUD
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> HUDWidgetClass;
    
    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UUserWidget* HUDWidget;
    
    virtual void BeginPlay() override;
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateHealth(float CurrentHealth, float MaxHealth);
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateScore(int32 Score);
};
