// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SteamFriendWidget.h"
#include "SteamFriendsList.generated.h"

/**
 * 
 */
class UScrollBox;
UCLASS()
class STARTERPROJECT_API USteamFriendsList : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Friends")
	void UpdateFriendsList(const TArray<FBPFriendInfo>& friendsList);

	void GetFriendsList();
	void OnReadFriendsListComplete(int32 LocalPlayerNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);
	TArray<FBPFriendInfo> SortFriends(const TArray<FBPFriendInfo>& friendsToSort);

	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (BindWidget))
	UScrollBox* ScrollBox_FriendContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friends")
	TSubclassOf<USteamFriendWidget> FriendWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "Friends")
	TArray<USteamFriendWidget*> FriendWidgets;
	
};
