// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameplayGameMode.generated.h"

class ABumperCar;

DECLARE_LOG_CATEGORY_EXTERN(LogGameplayGameMode, Log, All);
/**
 * 
 */
UCLASS()
class STARTERPROJECT_API AGameplayGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UFUNCTION() void KillPlayer(ABumperCar* DeadActor);
	void NotifyClientFinishedCountdown(APlayerController* PC);
	
	UFUNCTION(BlueprintCallable)
	void RequestedRespawn(ABumperCar* carToRespawn);

	UFUNCTION(BlueprintCallable) void RespawnAsGhost(ABumperCar* DeadActor);
	bool IsRoundInProgress() const {return bRoundInProgress	;};
	void ClientReady(APlayerController* PC);

	UFUNCTION(BlueprintImplementableEvent)
	void ShootCannonOnDeath();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayStartSequence();

	UFUNCTION(BlueprintImplementableEvent)
	void OnEndRoundEvent();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* ExitingController) override;

	UFUNCTION(BlueprintCallable)
	void RespawnAllCars();

	UFUNCTION(BlueprintCallable)
	void StartNextRound();

	UFUNCTION(BlueprintCallable)
	void OnStartSequenceFinished();

protected:
	AActor* ChoosePlayerStart_Implementation(AController* Player);
	virtual void HandleSeamlessTravelPlayer(AController*& C);
	void StartGameMatch();
	void CheckAllPlayersArrived();
	UFUNCTION() void OnPlayerDied(ABumperCar* DeadActor);

	UFUNCTION(BlueprintCallable)
	void RespawnCar(ABumperCar* carToRespawn);

	UPROPERTY(EditDefaultsOnly, Category = "Server Travel")
	FString EndScreenMapPath;

	UPROPERTY(BlueprintReadOnly) TArray<APlayerController*> ArrivedPlayers;
	TArray<APlayerController*> ClientsFinishedCountdown;

private:
	void OnAllPlayersArrived();
	void SetCarColor();
	void OnLevelLoaded();
	void StartCountdown();
	
	UFUNCTION() void EndRound(ABumperCar* winnerPtr);
	void OnMatchOver();
	void TravelToEndScreen();

	void DisplayPlayeDiedWidget(ABumperCar* DeadActor);
	UFUNCTION(BlueprintCallable)
	void NewRoundRespawnCar(ABumperCar* carToRespawn);

	

	UPROPERTY()	TArray<ABumperCar*> AlivePlayers;
	UPROPERTY()	TArray<ABumperCar*> DeadPlayers;
	TArray<APlayerController*> ReadyPlayers;

	UPROPERTY() int RoundNumber{ 1 };
	UPROPERTY(VisibleAnywhere) bool bRoundInProgress{ false };

	UPROPERTY(VisibleAnywhere) bool bRoundResetInProgress = false;

	/*FTimerHandle CountdownTimerHandle;
	int32 CountdownTime = 5;
	void StartDebugCountdown();
	void AdvanceDebugCountdown();*/

};
