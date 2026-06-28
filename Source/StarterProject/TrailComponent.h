// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrailComponent.generated.h"

class USplineComponent;
class UBumperCarMovementComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STARTERPROJECT_API UTrailComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTrailComponent();

	void ResetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void DeleteTimer(int index);

private:
	// SPLINE DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
	float CurrentSpawnFrequencyTimer = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
	float PointSpawnFrequency = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
	float MaxPointLifeTime = 1.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
	TArray<float> SplinePointLifeTime{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
	UBumperCarMovementComponent* MovementComponentPtr;

	UFUNCTION()
	void CheckIfPointNeedsToSpawn(float deltaTime);

	//UFUNCTION(Server, Reliable)
	//void Server_SpawnPoint(float deltaTime);

	UFUNCTION()
	void CheckIfPointIsDead(float deltaTime);
	
	//UFUNCTION(Server, Reliable)
	//void Server_DeletePoint(float deltaTime);
};
