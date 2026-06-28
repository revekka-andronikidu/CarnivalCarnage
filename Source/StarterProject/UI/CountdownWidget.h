// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/Delegate.h"
#include "CountdownWidget.generated.h"

class UAudioComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCountdownFinished);
/**
 * 
 */
UCLASS()
class STARTERPROJECT_API UCountdownWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCountdownFinished OnCountdownFinished;

    void StartCountDown();

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* CountdownAnim;

    float CountdownDuration;

private:
    UFUNCTION()
    void HandleAnimationFinished();

    UPROPERTY(EditAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAudioComponent> AudioComponentCountdownPtr;

    UPROPERTY(EditAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAudioComponent> AudioComponentMusicPtr;


    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* CountdownSound;

    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* GamePlayMusic;

};
