#include "MainMenuWidgetBase.h"

// Common UI
#include "CommonUI\Public\Widgets\CommonActivatableWidgetContainer.h"
#include "Components/Button.h"

// Framework
#include "Framework/GameInstances/CustomGameInstanceBase.h"
#include <Utils/Helpers/GameplayHelpers.h>
#include <Kismet/GameplayStatics.h>


DEFINE_LOG_CATEGORY(LogMainMenu);

void UMainMenuWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_CreateLobby)
	{
		Button_CreateLobby->OnClicked.AddDynamic(this, &UMainMenuWidgetBase::OnButton_CreateLobbyClicked);
	}
	if (Button_Quit)
	{
		Button_Quit->OnClicked.AddDynamic(this, &UMainMenuWidgetBase::OnButton_QuitClicked);
	}
	if (Button_Settings)
	{
		Button_Settings->OnClicked.AddDynamic(this, &UMainMenuWidgetBase::OnButton_SettingsClicked);
	}
}

bool UMainMenuWidgetBase::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	
	return true;
}

void UMainMenuWidgetBase::StartGame()
{
	auto gameInstance = GetGameInstance<UCustomGameInstanceBase>();
	if (!gameInstance)
	{
		UE_LOG(LogMainMenu, Error, TEXT("Could not find gameinstance..."));
		return;
	}

	//in case player is in a session, destroy it first
	//can happen if host from previous session quit and this player was kicked back to main menu
	//then tries to start a new game
	gameInstance->DestroySession();

#if WITH_EDITOR
	// Act as if we are session host
	gameInstance->Debug_SetSessionHost(true);

	// Load Level
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), DefaultLevel, true, FString(L"?listen"));
#else
	gameInstance->StartNewParty(OnlineSessionType, [=, this](bool wasSuccessfull) {
		if (!wasSuccessfull)
		{
			UE_LOG(LogMainMenu, Warning, TEXT("Could not start new game."));
			return;
		}

		// Load Level
		UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), DefaultLevel, true, FString(L"?listen"));
		});
#endif
}

void UMainMenuWidgetBase::QuitGame()
{
	// Quit Game
	FGenericPlatformMisc::RequestExit(false);
}


void UMainMenuWidgetBase::OnButton_QuitClicked()
{
	
	QuitGame();
}

void UMainMenuWidgetBase::OnButton_CreateLobbyClicked()
{
	StartGame();
}

void UMainMenuWidgetBase::OnButton_SettingsClicked()
{
	if (!SettingsWidgetRef) return;
	SettingsWidgetRef->AddToViewport();
}
