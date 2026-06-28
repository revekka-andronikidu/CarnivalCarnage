// Copyright Abonneer, Inc. All Rights Reserved.


#include "CountdownWidget.h"
#include "Components/AudioComponent.h"
#include "Animation/WidgetAnimation.h"
#include "StarterProject/States/GameplayGameState.h"

void UCountdownWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (!AudioComponentCountdownPtr)
    {
        AudioComponentCountdownPtr = NewObject<UAudioComponent>(this, UAudioComponent::StaticClass(), TEXT("CountdownAudio"));
        AudioComponentCountdownPtr->RegisterComponent();
        AudioComponentCountdownPtr->bAutoActivate = false;  // Don’t play automatically
    }
    if (!AudioComponentMusicPtr)
    {
        AudioComponentMusicPtr = NewObject<UAudioComponent>(this, UAudioComponent::StaticClass(), TEXT("CountdownAudio"));
        AudioComponentMusicPtr->RegisterComponent();
        AudioComponentMusicPtr->bAutoActivate = false;  // Don’t play automatically
    }

    if (CountdownAnim)
    {
        FWidgetAnimationDynamicEvent EndDelegate;
        EndDelegate.BindDynamic(this, &UCountdownWidget::HandleAnimationFinished);

        BindToAnimationFinished(CountdownAnim, EndDelegate);

        CountdownDuration = CountdownAnim->GetEndTime();
    }
}

void UCountdownWidget::HandleAnimationFinished()
{
    SetVisibility(ESlateVisibility::Hidden);
    UE_LOG(LogTemp, Log, TEXT("Animation finished"));
    AudioComponentMusicPtr->SetSound(GamePlayMusic);
    AudioComponentMusicPtr->Play();

    OnCountdownFinished.Broadcast();
}

void UCountdownWidget::StartCountDown()
{
    SetVisibility(ESlateVisibility::Visible);
    UE_LOG(LogTemp, Log, TEXT("Start countdown called"));

    auto GS = GetWorld()->GetGameState<AGameplayGameState>();
    if (!GS) return;

    float Now = GetWorld()->GetTimeSeconds();
    float Elapsed = Now - GS->GetCountdownStartTime(); 

    float PlaybackPosition = Elapsed; // Animation length is same as countdown

    // Ensure it's in bounds
    //PlaybackPosition = FMath::Clamp(PlaybackPosition, 0.f, CountdownDuration);

    UE_LOG(LogTemp, Log, TEXT("Coundown starting from %f:"), PlaybackPosition);

    // Start animation AT the correct time
    PlayAnimation(CountdownAnim, PlaybackPosition);


    if (AudioComponentCountdownPtr && CountdownSound)
    {
        AudioComponentCountdownPtr->SetSound(CountdownSound);
        AudioComponentCountdownPtr->Play(PlaybackPosition);
    }

    //if (AudioComponentMusicPtr && GamePlayMusic)
    //{
    //    AudioComponentMusicPtr->SetSound(GamePlayMusic);
    //}
}