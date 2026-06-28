// Copyright Abonneer, Inc. All Rights Reserved.


#include "GameControllerComponent.h"
#include "StarterProject/BumperCar.h"
#include "StarterProject/BumperCarMovementComponent.h"
#include "StarterProject/UI/CountdownWidget.h"
#include "StarterProject/GameModes/GameplayGameMode.h"

// Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values for this component's properties
UGameControllerComponent::UGameControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UGameControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameControllerComponent has no PlayerController owner!"));
		return;
	}
}

void UGameControllerComponent::SetUpInput()
{
	APlayerController* owningPC = Cast<APlayerController>(GetOwner());
	if (!owningPC || !owningPC->IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("GameControllerComponent has no PlayerController owner!!"));
		return;
	}
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(owningPC->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
	}

	if (UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(owningPC->InputComponent))
	{
		if (ABumperCar* BumperCar = Cast<ABumperCar>(owningPC->GetPawn()))
		{
			EnhancedInputComp->BindAction(SteeringActionPtr, ETriggerEvent::Triggered, BumperCar, &ABumperCar::SteeringAction);
			EnhancedInputComp->BindAction(SteeringActionPtr, ETriggerEvent::Completed, BumperCar, &ABumperCar::CompletedSteeringAction);
			EnhancedInputComp->BindAction(AccelerateActionPtr, ETriggerEvent::Triggered, BumperCar, &ABumperCar::AccelerateAction);
			EnhancedInputComp->BindAction(AccelerateActionPtr, ETriggerEvent::Completed, BumperCar, &ABumperCar::CompletedAccelerating);
			EnhancedInputComp->BindAction(BrakeActionPtr, ETriggerEvent::Triggered, BumperCar, &ABumperCar::BrakeAction);
			EnhancedInputComp->BindAction(BrakeActionPtr, ETriggerEvent::Completed, BumperCar, &ABumperCar::CompletedBraking);
			EnhancedInputComp->BindAction(ExtinguishActionPtr, ETriggerEvent::Completed, BumperCar, &ABumperCar::ServerExtinguishAction);
			EnhancedInputComp->BindAction(BoostActionPtr, ETriggerEvent::Triggered, BumperCar, &ABumperCar::BoostAction);
			EnhancedInputComp->BindAction(BoostActionPtr, ETriggerEvent::Completed, BumperCar, &ABumperCar::CompletedBoostAction);
			EnhancedInputComp->BindAction(CameraMovementAction, ETriggerEvent::Triggered, BumperCar, &ABumperCar::MoveCameraAction);
			EnhancedInputComp->BindAction(CameraMovementAction, ETriggerEvent::Completed, BumperCar, &ABumperCar::ResetCameraMovement);
			EnhancedInputComp->BindAction(CameraLookBackwardsAction, ETriggerEvent::Started, BumperCar, &ABumperCar::LookBackwards);
			EnhancedInputComp->BindAction(CameraLookBackwardsAction, ETriggerEvent::Completed, BumperCar, &ABumperCar::ResetBackWardsLookingCamera);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to cast to ABumperCar! - GameControllerComponent"));

		}
	}
}

void UGameControllerComponent::Client_DisableGameInput_Implementation()
{
	DisableGameInput();
}
void UGameControllerComponent::DisableGameInput()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	{
		subsystem->RemoveMappingContext(BumperCarContextPtr);
	}
}
void UGameControllerComponent::EnableGameInput()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	{
		subsystem->AddMappingContext(BumperCarContextPtr, 0);
	}
	PC->EnableInput(PC);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Green,
			FString::Printf(TEXT("Input enabled")));
	}
}

void UGameControllerComponent::Client_OnMatchStart_Implementation()
{
	EnableGameInput();

	//if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	//{
	//	if (ABumperCar* car = Cast<ABumperCar>(PC->GetPawn()))
	//	{
	//		car->DisableMovement(false);
	//	}
	//}
	Client_OnEnableGravity();
	
}

void UGameControllerComponent::Client_OnEnableGravity_Implementation()
{

	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (ABumperCar* car = Cast<ABumperCar>(PC->GetPawn()))
		{
			car->GetBumperCarMovementComponent()->DisableMovement(false);

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					2.0f,
					FColor::Green,
					FString::Printf(TEXT("Gravity enabled")));
			}
		}
	}

}


void UGameControllerComponent::OnCountdownReady()
{
	if (!CountdownWidget)
	{
		APlayerController* PC = Cast<APlayerController>(GetOwner());
		if (!PC) return;

		CountdownWidget = CreateWidget<UCountdownWidget>(PC, CountdownWidgetClass);
		UE_LOG(LogTemp, Log, TEXT("Countdown widget display created "));
		CountdownWidget->AddToViewport();
	}

	
		CountdownWidget->StartCountDown();
}


void UGameControllerComponent::Client_ShowCountdown_Implementation()
{
	if (!CountdownWidget)
	{
		APlayerController* PC = Cast<APlayerController>(GetOwner());
		if (!PC) return;

		CountdownWidget = CreateWidget<UCountdownWidget>(PC, CountdownWidgetClass);
		UE_LOG(LogTemp, Log, TEXT("Countdown widget display created "));
		CountdownWidget->AddToViewport();
	}

	UE_LOG(LogTemp, Log, TEXT("Countdown displayed!"));

	CountdownWidget->StartCountDown();
}

void UGameControllerComponent::SetUpComponent()
{
	SetUpInput();
	SetUpUI();
}
void UGameControllerComponent::SetUpUI()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return;

	if (CountdownWidgetClass)
	{
		CountdownWidget = CreateWidget<UCountdownWidget>(PC, CountdownWidgetClass);
		UE_LOG(LogTemp, Log, TEXT("Countdown widget display created "));

		if (CountdownWidget)
		{
			CountdownWidget->AddToViewport();
			CountdownWidget->SetVisibility(ESlateVisibility::Hidden);
			CountdownWidget->OnCountdownFinished.AddDynamic(this, &UGameControllerComponent::HandleCountdownFinished);
		}
	}
}

void UGameControllerComponent::HandleCountdownFinished()
{
	UE_LOG(LogTemp, Log, TEXT("Countdown finished - notifying server"));
	Server_NotifyCountdownFinished();
}

void UGameControllerComponent::Server_NotifyCountdownFinished_Implementation()
{
	AGameplayGameMode* GM = Cast<AGameplayGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->NotifyClientFinishedCountdown(Cast<APlayerController>(GetOwner()));
	}
}