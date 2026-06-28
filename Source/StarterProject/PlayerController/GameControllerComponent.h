// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameControllerComponent.generated.h"

class UInputMappingContext;
class UInputAction;
class UCountdownWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STARTERPROJECT_API UGameControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGameControllerComponent();

	UFUNCTION() void SetUpComponent();

	UFUNCTION(Client, Reliable)
	void Client_DisableGameInput();

	UFUNCTION(Client, Reliable)
	void Client_OnMatchStart();

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnEnableGravity();
	UFUNCTION(Client, Reliable)
	void Client_ShowCountdown();

	UFUNCTION(Server, Reliable)
	void Server_NotifyCountdownFinished();

	void OnCountdownReady();

	
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	UCountdownWidget* CountdownWidget;


	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UCountdownWidget> CountdownWidgetClass;



private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> BumperCarContextPtr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SteeringActionPtr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> AccelerateActionPtr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> BrakeActionPtr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ExtinguishActionPtr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> BoostActionPtr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CameraMovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CameraLookBackwardsAction;

	UFUNCTION() void EnableGameInput();
	UFUNCTION() void DisableGameInput();
	UFUNCTION() void SetUpInput();
	UFUNCTION() void SetUpUI();
	UFUNCTION()	void HandleCountdownFinished();
};
