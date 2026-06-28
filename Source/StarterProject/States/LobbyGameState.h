// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LobbyGameState.generated.h"

/**
 * 
 */
UCLASS()
class STARTERPROJECT_API ALobbyGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnPlayerJoined(bool bJoined);

protected:
	UFUNCTION()
	void OnPlayerJoined(bool bJoined);

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* JoinSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* LeftSound;

	
};
