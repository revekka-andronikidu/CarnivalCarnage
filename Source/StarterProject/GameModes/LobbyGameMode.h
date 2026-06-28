// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */

class ALobbyPlayerWindow;
class AMainPlayerController;
class ALobbyCharacterActor;

UCLASS()
class STARTERPROJECT_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

protected:

	ALobbyGameMode();

	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* ExitingController) override;

	UPROPERTY()
	TArray<APlayerController*> LobbyPlayerControllers;


	UPROPERTY(EditDefaultsOnly, Category = "Server Travel")
	FString MatchMapPath;

	UPROPERTY()
	TArray<ALobbyPlayerWindow*> PlayerWindows;

	UPROPERTY(BlueprintReadOnly)
	bool bSetupFinished = false;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Characters")
	TArray<TSubclassOf<ALobbyCharacterActor>> AllCharacterClasses;

public:
	UFUNCTION(BlueprintCallable) void StartGame();
	void OnReady(APlayerController* requesingPC, bool isReady);

	void CycleCharacter(APlayerController* RequestingPC, bool bNext);

private:
	void SetUpPlayerWindows();
	void RemoveDisconnectedPlayers();
	void CheckAndAssignNewPlayers();
	void UpdateWindows();
	void CheckAllPlayersReady();
	void ServerTravel();
	void AssignCharacterToPlayer(APlayerController* NewPlayer);
	void SwitchCharacter(APlayerController* RequestingPC, TSubclassOf<ALobbyCharacterActor> NewCharacter);

	UPROPERTY()
	APlayerController* HostController;

	UPROPERTY()
	TArray<TSubclassOf<ALobbyCharacterActor>> AvailableCharacters;

	UPROPERTY()
	TMap<APlayerController*, TSubclassOf<ALobbyCharacterActor>> AssignedCharacterMap;
};
