// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BumperCarState.h"
#include "BumperCarNeutralState.generated.h"

class ABumperCar;

/**
 * 
 */
UCLASS()
class STARTERPROJECT_API UBumperCarNeutralState : public UBumperCarState
{
	GENERATED_BODY()

public:
	UBumperCarNeutralState() = default;

	virtual TObjectPtr<UBumperCarState> HandleState(float deltaTime, AActor* ownerPtr);
	virtual void OnEnter(AActor* ownerPtr);
	virtual void OnExit(AActor* ownerPtr) {}

private:
	UFUNCTION()
	void OnFireTrailOverlap(AActor* overlappingActor);

	UFUNCTION()
	void OnSlimeTrailOverlap(AActor* overlappingActor);

	UFUNCTION()
	void SetupTrailEvents();

	UFUNCTION()
	void CalculateStartHealingTime(float deltaTime);

	UPROPERTY()
	bool bOnFire = false;

	UPROPERTY()
	bool bIsSlimed = false;

	UPROPERTY()
	bool bIsDead = false;

	UPROPERTY()
	bool bTrailEventsSetup = false;

	UPROPERTY()
	AActor* OwnerPtr;
	
	UPROPERTY()
	bool TimerStarted = false;

	UPROPERTY()
	float AccumulatedStartHealingTime = 0.f;

	UPROPERTY()
	float StartHealingTime = 5.f;
};
