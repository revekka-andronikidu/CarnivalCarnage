// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BumperCarState.h"
#include "BumperCarGhostState.generated.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(LogGhostState, Log, All);

UCLASS()
class STARTERPROJECT_API UBumperCarGhostState : public UBumperCarState
{
	GENERATED_BODY()

public:
	UBumperCarGhostState() = default;

	virtual TObjectPtr<UBumperCarState> HandleState(float deltaTime, AActor* ownerPtr);
	virtual void OnEnter(AActor* ownerPtr);
	virtual void OnExit(AActor*);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transform", meta = (AllowPrivateAccess = "true"))
	FVector CameraDeathLocation = FVector{ 0.f, 0.f, 500.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transform", meta = (AllowPrivateAccess = "true"))
	FRotator CameraDeathRotation = FRotator{ 0.f, 0.f, 0.f };
};
