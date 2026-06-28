// Copyright Abonneer, Inc. All Rights Reserved.


#include "EndScreenGameMode.h"
#include <Kismet/GameplayStatics.h>
#include "Framework/GameInstances/CustomGameInstanceBase.h"
#include "StarterProject/UI/LeaderboardWidget.h"
#include "StarterProject/EndScreen/LeaderboardActor.h"
#include "StarterProject//PlayerController/MainPlayerState.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameState.h"    
#include "StarterProject/PlayerController/MainPlayerController.h"
#include "Components/Image.h"
#include "StarterProject/States/EndScreenGameState.h"


void AEndScreenGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	UE_LOG(LogTemp, Log, TEXT("%s finished seamless travel"), *C->GetName());


	APlayerController* PC = Cast<APlayerController>(C);
	if (PC && !ArrivedPlayers.Contains(PC))
	{
		ArrivedPlayers.Add(PC);
		CheckAllPlayersArrived();
	}
}

void AEndScreenGameMode::CheckAllPlayersArrived()
{
	int32 expectedPlayers = 0;
	if (UCustomGameInstanceBase* GI = GetGameInstance<UCustomGameInstanceBase>())
	{
		expectedPlayers = GI->ExpectedPlayers;
	}

	if (ArrivedPlayers.Num() >= expectedPlayers && expectedPlayers > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("All %d players have joined the game!"), expectedPlayers);
		OnAllPlayersArrived();
	}
}

void AEndScreenGameMode::OnAllPlayersArrived()
{
	SetLeaderboardEntries();

	SetColors();

	for (auto PC : ArrivedPlayers)
	{
		AMainPlayerController* mainPC = Cast<AMainPlayerController>(PC);
		if (mainPC)
		{
			mainPC->Client_HideLoadingScreen();
		}
	}	
}

void AEndScreenGameMode::ExitGame()
{
	auto gameInstance = GetGameInstance<UCustomGameInstanceBase>();
	if (!gameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not find gameinstance..."));
		return;
	}

	gameInstance->DestroySession();

	//open main menu manually??
	//there should be automatic fall back when there is a session
	//added only for debug now
    UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), MainMenu, true);
}


void AEndScreenGameMode::BeginPlay()
{
	Super::BeginPlay();


	

    //SetColors();
}

void AEndScreenGameMode::SetColors()
{
   SetWinnerColorEvent(LeaderboardEntries[0].Color);

   TArray<float> Colors;
   for(int i = 1; i < LeaderboardEntries.Num(); i++)
   {
       Colors.Add(LeaderboardEntries[i].Color);
   }

   SetLooserPlayerColors(Colors);
}


void AEndScreenGameMode::SetLeaderboardEntries()
{
	LeaderboardEntries.Empty();

	for (auto PC : ArrivedPlayers)
	{
		AMainPlayerState* playerState = PC->GetPlayerState<AMainPlayerState>();
		if (!playerState) continue;

		LeaderboardEntries.Add(playerState->GetLeaderboardEntry());
	}

	LeaderboardEntries.Sort([](const FLeaderboardEntry& A, const FLeaderboardEntry& B)
		{
			return A.PlayerScore > B.PlayerScore;
		});

	SetLeaderboard();
}
