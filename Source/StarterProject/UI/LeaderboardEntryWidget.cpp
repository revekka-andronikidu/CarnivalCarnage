// Copyright Abonneer, Inc. All Rights Reserved.


#include "LeaderboardEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "StarterProject/PlayerController/MainPlayerState.h"

void ULeaderboardEntryWidget::UpdateEntry(const FLeaderboardEntry& entry)
{
	//UE_LOG(LogTemp, Log, TEXT("Update for Leaderboard widget called."));

	PlayerName = entry.PlayerName;
	Wins = entry.PlayerScore;
	AvatarTexture = entry.AvatarTexture;

	if(T_PlayerName)
	T_PlayerName->SetText(FText::FromString(PlayerName));

	if(T_PlayerScore)
	T_PlayerScore->SetText(FText::AsNumber(Wins));

	OnLeaderboardEntryUpdated();
}