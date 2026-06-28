// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LoadingControllerComponent.generated.h"


UCLASS( ClassGroup=(UI), meta=(BlueprintSpawnableComponent) )
class STARTERPROJECT_API ULoadingControllerComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loading Screen")
    FSlateBrush BackgroundBrush;

public:
    ULoadingControllerComponent();

    UFUNCTION(BlueprintCallable)
    void ShowLoadingScreen();

    UFUNCTION(BlueprintCallable)
    void RequestHideLoadingScreen();



private:
    TSharedPtr<SWidget> SlateLoadingScreenWidget;
   void InitializeLoadingScreen();

   void HideLoadingScreen();


   float LoadingTimer = 0.f;
   float MinLoadingTime = 3.f;
   bool bHideRequested = false;
   bool bIsLoadingScreenVisible = false;
		
};
