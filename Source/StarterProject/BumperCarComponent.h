// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "States/BumperCarState.h"
#include "BumperCarComponent.generated.h"

class UBumperCarState;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STARTERPROJECT_API UBumperCarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBumperCarComponent();

	UFUNCTION()
	void ResetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TObjectPtr<UBumperCarState> GetState() const { return StatePtr; }	
	EBumperCarStateType GetStateType() const;

	void SetState(TSubclassOf<UBumperCarState> NewStateClass);

private:
	// STATES
	UFUNCTION() void HandleState(float deltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBumperCarState> StatePtr;

	UPROPERTY(Replicated)
	EBumperCarStateType StateType;


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
