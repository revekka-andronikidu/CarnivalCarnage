// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyControllerComponent.h"
#include "GameControllerComponent.h"
#include "LoadingControllerComponent.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */

UCLASS()
class STARTERPROJECT_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainPlayerController();
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	//virtual void OnUnPossess() override;

	void SwitchToLobby();
	void SwitchToGame();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	ULobbyControllerComponent* LobbyComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UGameControllerComponent* GameComponent;

	void ShowLoadingScreen();
	UFUNCTION(BlueprintImplementableEvent)
	void OnSwitchToGame();

	UFUNCTION(Client, Reliable)
	void Client_ShowPlayerDiedWidget(const FString& deadPlayerName);

	UFUNCTION(Client, Reliable)
	void Client_ShowLoadingScreen();

	UFUNCTION(Client, Reliable)
	void Client_HideLoadingScreen();


	UFUNCTION(Server, Reliable)
	void Server_NotifyClientReady();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	ULoadingControllerComponent* LoadingComponent;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowPlayerDiedWidget(const FString& deadPlayerName);


	void StartCheckingMapReady();
	bool CheckIfMapLoaded();
};
