// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LobbyControllerComponent.generated.h"


class UInputMappingContext;
class UInputAction;
class ULobbyUIWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STARTERPROJECT_API ULobbyControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULobbyControllerComponent();

	UFUNCTION(Client, Reliable)
	void Client_OnGameStart();

	void SetupLobbyUI();
	void EnableStartButton(bool playersReady);

	UFUNCTION(BlueprintCallable)
	void RequestReadyToggle();

	UPROPERTY(BlueprintReadOnly)
	ULobbyUIWidget* LobbyUIRef;

	bool GetIsReady() { return IsReady; };

	void SetupLobbyInput();


protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void BeginPlay() override;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<ULobbyUIWidget> LobbyWidgetClass;*/
	UFUNCTION(BlueprintCallable)
	void OnMatchStart();
	UFUNCTION(Server, Reliable)
	void Server_RequestReadyToggle();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> LobbyInputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CycleLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CycleRightAction;
	
	
private:
	UPROPERTY()
	bool IsReady = false;		

	void OnCycleLeft();
	void OnCycleRight();
};
