// Copyright Abonneer, Inc. All Rights Reserved.


#include "SteamFriendsList.h"
#include "Components/ScrollBox.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Online/AdvancedSessions/AdvancedFriendsLibrary.h"
#include "Online/AdvancedSteamSessions/AdvancedSteamFriendsLibrary.h"

void USteamFriendsList::NativeConstruct()
{
	Super::NativeConstruct();

	//GetFriendsList();
}

void USteamFriendsList::GetFriendsList()
{
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub)
    {
        /*IOnlineFriendsPtr FriendsInterface = OnlineSub->GetFriendsInterface();
        if (FriendsInterface.IsValid())
        {
          FriendsInterface->ReadFriendsList(0, EFriendsLists::ToString(EFriendsLists::Default), FOnReadFriendsListComplete::CreateUObject(this, &USteamFriendsList::OnReadFriendsListComplete));
        }*/
    }
}

void USteamFriendsList::OnReadFriendsListComplete(int32 LocalPlayerNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
    /*if (bWasSuccessful)
    {
        IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
        IOnlineFriendsPtr FriendsInterface = OnlineSub->GetFriendsInterface();
        TArray<TSharedRef<FOnlineFriend>> Friends;
        if (FriendsInterface.IsValid())
        {
            FriendsInterface->GetFriendsList(LocalPlayerNum, ListName, Friends);

            for (auto& Friend : Friends)
            {
                UE_LOG(LogTemp, Log, TEXT("Friend: %s"), *Friend->GetDisplayName());
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("Friend: %s"), *Friend->GetDisplayName()));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get friends list: %s"), *ErrorStr);
    }*/
}

TArray<FBPFriendInfo> USteamFriendsList::SortFriends(const TArray<FBPFriendInfo>& friendsToSort)
{

	//TArray<FBPFriendInfo> sortedFriends = friendsToSort;
	//
	//std::stable_sort(sortedFriends.begin(), sortedFriends.end(), [](const FBPFriendInfo A, const FBPFriendInfo B)
	//	{
	//		const bool aOffline = (A.OnlineState == EBPOnlinePresenceState::Offline);
	//		const bool bOffline = (B.OnlineState == EBPOnlinePresenceState::Offline);
	//
	//		if (aOffline != bOffline)
	//		{
	//			return (!aOffline && bOffline); // Online first
	//		}
	//
	//		// Both same status -> sort by name
	//		return A.DisplayName < B.DisplayName;
	//	});
	//
	//return sortedFriends;
	
	TArray<FBPFriendInfo> sortedFriends;
	sortedFriends.Empty();

	TArray<FBPFriendInfo> offlineFriends;
	
	for (auto steamFriend : friendsToSort)
	{
		if (steamFriend.OnlineState == EBPOnlinePresenceState::Offline)
		{
			offlineFriends.Add(steamFriend);
		}
		else
		{
			sortedFriends.Add(steamFriend);
		}
	}
	sortedFriends.Append(offlineFriends);


	/*GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Green,
		FString::Printf(TEXT("Sorted friends count: %d"), sortedFriends.Num())
	);*/

	return sortedFriends;
}

void USteamFriendsList::UpdateFriendsList(const TArray<FBPFriendInfo>& friendsList)
{
	TArray<FBPFriendInfo> friends = SortFriends(friendsList);
	if (friends.Num() <= 0)
		return;

	ScrollBox_FriendContainer->ClearChildren();

	for (int32 i = 0; i < friends.Num(); i++)
	{
		if (FriendWidgets.Num() < friends.Num())
		{
			USteamFriendWidget* newFriendWidget = CreateWidget<USteamFriendWidget>(GetWorld(), FriendWidgetClass);
			FriendWidgets.Add(newFriendWidget);
		}

		FriendWidgets[i]->UpdateData(friends[i]);
		ScrollBox_FriendContainer->AddChild(FriendWidgets[i]);
	}

}
