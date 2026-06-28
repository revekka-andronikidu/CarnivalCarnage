// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BumperCarState.h"
#include "BumperCarOnSlimedState.generated.h"

class ABumperCar;

UCLASS()
class STARTERPROJECT_API UBumperCarOnSlimedState : public UBumperCarState
{
	GENERATED_BODY()

public:
	UBumperCarOnSlimedState() = default;

	virtual TObjectPtr<UBumperCarState> HandleState(float deltaTime, AActor* ownerPtr);
	virtual void OnEnter(AActor* ownerPtr);
	virtual void OnExit(AActor* ownerPtr);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Owner", meta = (AllowPrivateAccess = "true"))
	ABumperCar* OwnerPtr{};
	
	UFUNCTION()	void OnFireTrailOverlap(AActor* overlappingActor);

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dead", meta = (AllowPrivateAccess = "true"))
	bool bIsDead = false;*/

	float SlimedTimeElapsed = 0.f;
	float TotalSlimedTime = 1.5f;

	UPROPERTY()	bool bOnFire = false;
	UPROPERTY()	bool bTrailEventsSetup = false;
	UFUNCTION() void SetupTrailEvent();
};

