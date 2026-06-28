// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Online/AdvancedSessions/AdvancedFriendsLibrary.h"
#include "SteamFriendWidget.generated.h"

/**
 * 
 */
class UButton;
class UImage;
class UTextBlock;

UCLASS()
class STARTERPROJECT_API USteamFriendWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateData(FBPFriendInfo steamFriendData);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButton_InviteClicked();

	UFUNCTION(BlueprintCallable)
	void UpdateTab(FBPFriendInfo steamFriendData);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friends")
	FBPFriendInfo FriendData;
	

	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_PlayerName;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_Invite;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* Image_Avatar;
	
};
