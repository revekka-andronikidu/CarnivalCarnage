// Copyright Abonneer, Inc. All Rights Reserved.


#include "LoadingControllerComponent.h"
#include "Blueprint/UserWidget.h"
#include "Framework/GameInstances/CustomGameInstanceBase.h"
#include "Engine/GameInstance.h"

// Sets default values for this component's properties
ULoadingControllerComponent::ULoadingControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULoadingControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsLoadingScreenVisible)
    {
        LoadingTimer += DeltaTime;

        // If Hide was requested & min time reached hide now
        if (bHideRequested && LoadingTimer >= MinLoadingTime)
        {
            HideLoadingScreen();
        }
    }

}


void ULoadingControllerComponent::BeginPlay()
{
    Super::BeginPlay();

    InitializeLoadingScreen();
}

void ULoadingControllerComponent::ShowLoadingScreen()
{
    GEngine->GameViewport->AddViewportWidgetContent(SlateLoadingScreenWidget.ToSharedRef(), 1000);

    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC) return;

        PC->SetInputMode(FInputModeUIOnly());
        PC->bShowMouseCursor = false;

    LoadingTimer = 0;
    bIsLoadingScreenVisible = true;
    bHideRequested = false;

}

void ULoadingControllerComponent::HideLoadingScreen()
{
    if (GEngine && GEngine->GameViewport && SlateLoadingScreenWidget.IsValid())
    {
        GEngine->GameViewport->RemoveViewportWidgetContent(SlateLoadingScreenWidget.ToSharedRef());
    }

    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (PC)
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
    }

    LoadingTimer = 0;
    bIsLoadingScreenVisible = false;
    bHideRequested = false;
}

void ULoadingControllerComponent::RequestHideLoadingScreen()
{
    if (bIsLoadingScreenVisible && LoadingTimer < MinLoadingTime)
    {
        bHideRequested = true;
        return;
    }

    // Otherwise hide immediately
    HideLoadingScreen();
}

void ULoadingControllerComponent::InitializeLoadingScreen()
{
    if (SlateLoadingScreenWidget.IsValid())
    {
        return; // Already created
    }


    SlateLoadingScreenWidget = SNew(SOverlay)

        // Fullscreen colored background layer
        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SImage)
                .Image(&BackgroundBrush)
        ]
        ;

        //// Centered text layer
        //+ SOverlay::Slot()
        //.HAlign(HAlign_Center)
        //.VAlign(VAlign_Center)
        //[
        //    SNew(STextBlock)
        //        .Text(FText::FromString(TEXT("Loading...")))
        //        .ColorAndOpacity(FSlateColor(FLinearColor::White))
        //        .Font(FCoreStyle::Get().GetFontStyle("NormalFont"))
        //];
}
