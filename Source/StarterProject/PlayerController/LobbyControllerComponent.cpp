// Copyright Abonneer, Inc. All Rights Reserved.


#include "LobbyControllerComponent.h"
#include "MainPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "StarterProject/GameModes/LobbyGameMode.h" 
#include "Components/Button.h"
#include "Components/WidgetComponent.h"
#include "Framework/GameInstances/CustomGameInstanceBase.h"
#include "StarterProject/UI/LobbyWidgetActor.h"
#include "EngineUtils.h"
#include "StarterProject/UI/LobbyUIWidget.h"

// Input
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"


// Sets default values for this component's properties
ULobbyControllerComponent::ULobbyControllerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void ULobbyControllerComponent::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC) return;

    if (PC->HasAuthority() && PC->IsLocalController())
    {
        IsReady = true;
    }
}

void ULobbyControllerComponent::SetupLobbyUI()
{
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("LobbyControllerComponent has no PlayerController owner!"));
        return;
    }


    if (PC->IsLocalController())
    {
       /* if (LobbyWidgetClass)
        {
            LobbyUIRef = CreateWidget<ULobbyUIWidget>(PC, LobbyWidgetClass);
            if (LobbyUIRef)
            {
                LobbyUIRef->AddToViewport();
            }
        }*/

        ALobbyWidgetActor* LobbyWidgetActorRef = nullptr;

        for (TActorIterator<ALobbyWidgetActor> It(GetWorld()); It; ++It)
        {
            LobbyWidgetActorRef = *It;
            break;
        }

        if (LobbyWidgetActorRef)
        {
            UWidgetComponent* WidgetComp = LobbyWidgetActorRef->GetComponentByClass<UWidgetComponent>();
            if (WidgetComp)
            {
               LobbyUIRef = Cast<ULobbyUIWidget>(WidgetComp->GetWidget());
            }
        }

        PC->bShowMouseCursor = true;
        FInputModeGameAndUI InputMode;
        PC->SetInputMode(InputMode);
    }
   
}


void ULobbyControllerComponent::Client_OnGameStart_Implementation()
{
    OnMatchStart();
}

void ULobbyControllerComponent::OnMatchStart()
{
    if (!LobbyUIRef) return;

    //Show loading screen
    AMainPlayerController* PC = Cast<AMainPlayerController>(GetOwner());
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("LobbyControllerComponent has no PlayerController owner!"));
        return;
    }   
    
    PC->ShowLoadingScreen();
    PC->DisableInput(PC);
    PC->bShowMouseCursor = false;
}

void ULobbyControllerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (LobbyUIRef)
    {
        LobbyUIRef->RemoveFromParent();
        LobbyUIRef = nullptr;
    }
}

void ULobbyControllerComponent::RequestReadyToggle()
{
    // This runs on the CLIENT when the Ready button is clicked
    if (GetOwnerRole() == ROLE_Authority)
    {
        // Already on server (listen server case)

        Server_RequestReadyToggle();
    }
    else
    {
        // Ask the server to execute the RPC
        UE_LOG(LogTemp, Log, TEXT("[Client] Sending RPC to server"));
        Server_RequestReadyToggle();
    }
}

void ULobbyControllerComponent::Server_RequestReadyToggle_Implementation()
{
    UWorld* World = GetWorld();
    if (!World) return;

    ALobbyGameMode* LobbyGM = Cast<ALobbyGameMode>(UGameplayStatics::GetGameMode(World));
    if (!LobbyGM) return;

    APlayerController* OwnerPC = Cast<APlayerController>(GetOwner());
    if (!OwnerPC) return;

    IsReady = !IsReady;

    LobbyGM->OnReady(OwnerPC, IsReady);
}


void ULobbyControllerComponent::EnableStartButton(bool playersReady)
{
    LobbyUIRef->ToggleStartEnabled(playersReady);
}

void ULobbyControllerComponent::SetupLobbyInput()
{
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC || !PC->IsLocalController()) return;

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
    {
        Subsystem->ClearAllMappings(); // optional, depends if you want only lobby input active
        Subsystem->AddMappingContext(LobbyInputMappingContext, 1); // LobbyInputMappingContext is a UInputMappingContext*
    }

    if (UEnhancedInputComponent* InputComp = CastChecked<UEnhancedInputComponent>(PC->InputComponent))
    {
        InputComp->BindAction(CycleLeftAction, ETriggerEvent::Started, this, &ULobbyControllerComponent::OnCycleLeft);
        InputComp->BindAction(CycleRightAction, ETriggerEvent::Started, this, &ULobbyControllerComponent::OnCycleRight);
    }
}

void ULobbyControllerComponent::OnCycleLeft()
{
    if (ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
    {
        GM->CycleCharacter(Cast<APlayerController>(GetOwner()), false);
        
    }
}

void ULobbyControllerComponent::OnCycleRight()
{
    if (ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
    {
        GM->CycleCharacter(Cast<APlayerController>(GetOwner()), true);
    }
}

