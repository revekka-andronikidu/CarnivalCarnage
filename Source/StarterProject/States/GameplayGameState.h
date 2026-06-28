// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameplayGameState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeaderboardChanged);

UCLASS()
class STARTERPROJECT_API AGameplayGameState : public AGameState
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	void SetCountdownStartTime(float time);
	float GetCountdownStartTime() const { return CountdownStartTime; };

	UPROPERTY(ReplicatedUsing = OnRep_RoundInProgress)
	bool bRoundInProgress{ false };

	UPROPERTY(BlueprintAssignable)
	FOnLeaderboardChanged OnLeaderboardChanged;

	private:
	UPROPERTY(ReplicatedUsing = OnRep_CountdownStart)
	float CountdownStartTime;

	UFUNCTION() void OnRep_CountdownStart();

	UFUNCTION()
	void OnRep_RoundInProgress();

};
