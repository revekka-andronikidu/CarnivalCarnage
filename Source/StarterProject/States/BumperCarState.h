// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BumperCarState.generated.h"

UENUM(BlueprintType)
enum class EBumperCarStateType : uint8
{
	Neutral,
	OnFire,
	Ghost,
	None,
};

UCLASS()
class STARTERPROJECT_API UBumperCarState : public UObject
{
	GENERATED_BODY()

public:
	virtual TObjectPtr<UBumperCarState> HandleState(float, AActor*) { return nullptr; }
	virtual void OnEnter(AActor*) {}
	virtual void OnExit(AActor*) {}

	virtual EBumperCarStateType GetType() const { return StateType; }

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	EBumperCarStateType StateType;
};
