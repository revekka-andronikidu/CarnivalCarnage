// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BumperCarState.h"
#include "BumperCarOnFireState.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOnFireState, Log, All);

class ABumperCar;

/**
 * 
 */
UCLASS()
class STARTERPROJECT_API UBumperCarOnFireState : public UBumperCarState
{
	GENERATED_BODY()

public:
	UBumperCarOnFireState() = default;

	virtual TObjectPtr<UBumperCarState> HandleState(float deltaTime, AActor* ownerPtr);
	virtual void OnEnter(AActor* ownerPtr);
	virtual void OnExit(AActor* ownerPtr);

	UFUNCTION()
	void ExtinguishButtonPressed();
	
private:
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	bool bIsDead = false;*/
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	float DamagePerSecond = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	float CurrentDamageTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extinguish", meta = (AllowPrivateAccess = "true"))
	int ButtonPressCounter = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extinguish", meta = (AllowPrivateAccess = "true"))
	int AmountOfPressesToExtinguish = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extinguish", meta = (AllowPrivateAccess = "true"))
	float DeltaTimeBetweenPresses = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extinguish", meta = (AllowPrivateAccess = "true"))
	float MaxTimeBetweenPresses = 0.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Owner", meta = (AllowPrivateAccess = "true"))
	ABumperCar* OwnerPtr{};

	

	UFUNCTION()
	bool CheckButtonPressTimer(float deltaTime);

	UFUNCTION()
	void ResetButtonPresses();

	UFUNCTION()
	void CalcDamage(float deltaTime);

	UFUNCTION(Server, Reliable)
	void ServerRegisterExtinguishPress();


	
	


	//UFUNCTION()
	//void OnDeath() { bIsDead = true; }
};
