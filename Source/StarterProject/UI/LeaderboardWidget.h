// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StarterProject/PlayerController/MainPlayerState.h"
#include "LeaderboardWidget.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */

UCLASS()
class STARTERPROJECT_API ULeaderboardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	TArray<FLeaderboardEntry> GetEntried() const { return Entries; };

	UFUNCTION(BlueprintCallable) void RefreshLeaderboard();

protected:

	UPROPERTY(BlueprintReadOnly) TArray<FLeaderboardEntry> Entries;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateLeaderboard();
	
};
