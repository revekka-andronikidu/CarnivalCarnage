// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "EndScreenGameMode.generated.h"

class ULeaderboardWidget;
class ALeaderboardActor;
struct FLeaderboardEntry;
/**
 * 
 */
UCLASS()
class STARTERPROJECT_API AEndScreenGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	void ExitGame();

	UFUNCTION(BlueprintCallable)
	void SetColors();
protected:
	virtual void HandleSeamlessTravelPlayer(AController*& C);
	void CheckAllPlayersArrived();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void SetWinnerColorEvent(float color);

	UFUNCTION(BlueprintImplementableEvent)
	void SetLeaderboard();
	void SetLeaderboardEntries();

	UFUNCTION(BlueprintImplementableEvent)
	void SetLooserPlayerColors(const TArray<float>& colors);

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> MainMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLeaderboardEntry> LeaderboardEntries;


	UPROPERTY(BlueprintReadOnly) TArray<APlayerController*> ArrivedPlayers;

private:
	
	void OnAllPlayersArrived();
};
