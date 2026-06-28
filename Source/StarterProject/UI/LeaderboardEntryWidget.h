// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LeaderboardEntryWidget.generated.h"

/**
 * 
 */
class UTextBlock;
class UImage;
struct FLeaderboardEntry;

UCLASS()
class STARTERPROJECT_API ULeaderboardEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable) void UpdateEntry(const FLeaderboardEntry& entry);
	
protected:
	UFUNCTION(BlueprintImplementableEvent) 
	void OnLeaderboardEntryUpdated();

	UPROPERTY(meta = (BindWidget)) UTextBlock* T_PlayerName;
	UPROPERTY(meta = (BindWidget)) UTextBlock* T_PlayerScore;
	//UPROPERTY(meta = (BindWidget)) UImage* Image_Avatar;

	UPROPERTY()	FString PlayerName;
	UPROPERTY()	int32 Wins;
	UPROPERTY(BlueprintReadOnly)	UTexture2D* AvatarTexture;
};
