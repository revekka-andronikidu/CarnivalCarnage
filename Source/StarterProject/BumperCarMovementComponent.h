// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BumperCarMovementComponent.generated.h"

class ABumperCar;

DECLARE_LOG_CATEGORY_EXTERN(LogMovementComponent, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STARTERPROJECT_API UBumperCarMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBumperCarMovementComponent();

	void ResetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// INPUT FUNCTIONS
	UFUNCTION() void Accelerate(float inputValue, bool isCompleted);
	UFUNCTION() void Brake(float inputValue, bool isCompleted);
	UFUNCTION() void Steer(float inputValue);
	UFUNCTION() void Boost(bool isCompleted);
	
	// MOVEMENT DISABLED FUNCTIONS
	UFUNCTION() void DisableMovementBySeconds(float seconds);
	UFUNCTION() void DisableMovement(bool Enabled);
	UFUNCTION() bool GetMovementEnabled() const;
	UFUNCTION() void DisableAcceleration(float seconds);
	UFUNCTION() void DisableSteering(bool bSteeringEnabled);
	UFUNCTION() void IsSlimed(float seconds);

	// CUSTOM PHYSICS FUNCTIONS
	UFUNCTION() void Impulse(FVector force);

	// GETTERS
	UFUNCTION() FVector GetVelocity();
	UFUNCTION() float GetCurrentSpeed() const;
	UFUNCTION() float GetMaxSpeed() const { return MaxSpeed; }
	UFUNCTION() float GetMaxBoostSpeed() const { return MaxBoostSpeed; }
	UFUNCTION() bool GetIsBraking() const { return bIsBraking; }
	UFUNCTION() bool GetIsSteering() const { return bIsSteering; }
	UFUNCTION() bool GetIsSteeringRight() const { return bIsSteeringRight; }
	UFUNCTION() float GetSteeringSpeed() const { return CurrentSteeringSpeed; }
	UFUNCTION() float GetMaxSteeringSpeed() const { return MaxSteeringSpeed; }
	UFUNCTION() float GetMaxReverseSpeed() const { return MaxReverseSpeed; }

private:
	// SPEED
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement | Speed", meta = (AllowPrivateAccess = "true"))
	float CurrentSpeed = 0.f;

	UPROPERTY(ReplicatedUsing=OnRep_SetCurrentSpeed)
	float ReplicatedCurrentSpeed = 0.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Speed", meta = (AllowPrivateAccess = "true"))
	float MinSpeed = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Movement | Speed", meta = (AllowPrivateAccess = "true"))
	float MaxSpeed = 2500.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Speed", meta = (AllowPrivateAccess = "true"))
	float PerpetualMinSpeed = 400.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Speed", meta = (AllowPrivateAccess = "true"))
	float MaxReverseSpeed = -1500.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement | Speed", meta = (AllowPrivateAccess = "true"))
	bool bIsReversing = false;

	// BOOST
	UPROPERTY(EditAnywhere, Category = "Movement | Boost", meta = (AllowPrivateAccess = "true"))
	bool bIsBoosting = false;

	UPROPERTY(EditAnywhere, Category = "Movement | Boost", meta = (AllowPrivateAccess = "true"))
	float MaxBoostSpeed = 3500.f;

	// VELOCITY CALCULATION
	UPROPERTY(VisibleAnywhere, Category = "Movement | Velocity", meta = (AllowPrivateAccess = "true"))
	FVector LastPosition = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Category = "Movement | Velocity", meta = (AllowPrivateAccess = "true"))
	float LastDeltaTime = 0.f;

	// ACCELERATION & DECELERATION	
	UPROPERTY(EditAnywhere, Category = "Movement | Acceleration", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating = false;
	
	UPROPERTY(EditAnywhere, Category = "Movement | Acceleration", meta = (AllowPrivateAccess = "true"))
	float CurrentAcceleration = 0.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Acceleration", meta = (AllowPrivateAccess = "true"))
	float AccelerationStep = 200.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Acceleration", meta = (AllowPrivateAccess = "true"))
	float BoostAccelerationStep = 2000.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Acceleration", meta = (AllowPrivateAccess = "true"))
	float MinAcceleration = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Movement | Acceleration", meta = (AllowPrivateAccess = "true"))
	float MaxAcceleration = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Acceleration", meta = (AllowPrivateAccess = "true"))
	float BoostMaxAcceleration = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Deceleration", meta = (AllowPrivateAccess = "true"))
	float DecelerationSpeed = 500.f;

	// BRAKING
	UPROPERTY(EditAnywhere, Category = "Movement | Brake", meta = (AllowPrivateAccess = "true"))
	bool bIsBraking = false;
	
	UPROPERTY(EditAnywhere, Category = "Movement | Brake", meta = (AllowPrivateAccess = "true"))
	float MinBrakeForce = 0.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Brake", meta = (AllowPrivateAccess = "true"))
	float MaxBrakeForce = -1500.f;

	// STEERING
	UPROPERTY(EditAnywhere, Category = "Movement | Steering", meta = (AllowPrivateAccess = "true"))
	bool bSteeringDisabled = false;

	UPROPERTY(EditAnywhere, Category = "Movement | Steering", meta = (AllowPrivateAccess = "true"))
	bool bIsSteering = false;

	UPROPERTY(EditAnywhere, Category = "Movement | Steering", meta = (AllowPrivateAccess = "true"))
	bool bIsSteeringRight = false;

	UPROPERTY(EditAnywhere, Category = "Movement | Steering", meta = (AllowPrivateAccess = "true"))
	float CurrentSteeringSpeed = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Movement | Steering", meta = (AllowPrivateAccess = "true"))
	float MinSteeringSpeed = 90.f;
	
	UPROPERTY(EditAnywhere, Category = "Movement | Steering", meta = (AllowPrivateAccess = "true"))
	float MaxSteeringSpeed = 120.f; // degrees per second

	UPROPERTY(EditAnywhere, Category = "Movement | Steering", meta = (AllowPrivateAccess = "true"))
	float SpeedToBaseSteering = 1250.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Rotation", meta = (AllowPrivateAccess = "true"))
	FRotator CurrentRotation = FRotator::ZeroRotator;
	
	UPROPERTY(EditAnywhere, Category = "Movement | Rotation", meta = (AllowPrivateAccess = "true"))
	FRotator DesiredRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "Movement | Rotation", meta = (AllowPrivateAccess = "true"))
	float RotationInterpolationSpeed = 15.f;

	// DRIFTING
	UPROPERTY(EditAnywhere, Category = "Movement | Drifting", meta = (AllowPrivateAccess = "true"))
	bool bIsDrifting = false;

	UPROPERTY(EditAnywhere, Category = "Movement | Drifting", meta = (AllowPrivateAccess = "true"))
	float DriftSteeringSpeed = 120.f; // degrees per second

	UPROPERTY(EditAnywhere, Category = "Movement | Drifting", meta = (AllowPrivateAccess = "true"))
	float CurrentRotationAngle = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Movement | Drifting", meta = (AllowPrivateAccess = "true"))
	float MaxRotationAngle = 30.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Drifting", meta = (AllowPrivateAccess = "true"))
	float MaxDriftingSpeed = 1500.f;

	// PHYSICS
	UPROPERTY(EditAnywhere, Category = "Movement | Physics", meta = (AllowPrivateAccess = "true"))
	FVector ImpulseVectorForce = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Movement | Physics", meta = (AllowPrivateAccess = "true"))
	float DragFactor = 0.95f;

	UPROPERTY(EditAnywhere, Category = "Movement | Physics", meta = (AllowPrivateAccess = "true"))
	float CounterDampingFactor = 0.90f;

	// SETTINGS
	UPROPERTY(EditAnywhere, Category = "Movement | Disable Acceleration", meta = (AllowPrivateAccess = "true"))
	bool bForceDisableAcceleration = false;

	UPROPERTY(EditAnywhere, Category = "Movement | Disable Acceleration", meta = (AllowPrivateAccess = "true"))
	float AccumulatedDisabledAccelerationTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Disable Acceleration", meta = (AllowPrivateAccess = "true"))
	float DisabledAccelerationTime = 1.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Disable Movement", meta = (AllowPrivateAccess = "true"))
	bool bForceDisableMovement = true;
	
	UPROPERTY(EditAnywhere, Category = "Movement | Disable Movement", meta = (AllowPrivateAccess = "true"))
	bool bMovementEnabled = true;

	UPROPERTY(EditAnywhere, Category = "Movement | Disable Movement", meta = (AllowPrivateAccess = "true"))
	float AccumulatedDisabledTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Disable Movement", meta = (AllowPrivateAccess = "true"))
	float DisabledTime = 1.f;

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bPerpetualDriving = false;

	UPROPERTY(EditAnywhere, Category = "Movement | Ground Check", meta = (AllowPrivateAccess = "true"))
	float GroundCheckDistance = 50.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Ground Check", meta = (AllowPrivateAccess = "true"))
	float GravityForce = 981.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Ground Check", meta = (AllowPrivateAccess = "true"))
	float GravityMultiplier = 2.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Ground Check", meta = (AllowPrivateAccess = "true"))
	bool bIsFalling = false;

	UPROPERTY(EditAnywhere, Category = "Movement | Ground Snapping", meta = (AllowPrivateAccess = "true"))
	float SnappingOffset = 10.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Ground Snapping", meta = (AllowPrivateAccess = "true"))
	float SnappingInterpolationSpeed = 15.f;

	// FALLING
	UPROPERTY(VisibleAnywhere, Category = "Movement | Falling", meta = (AllowPrivateAccess = "true"))
	bool bFallingDirectionSetup = false;
	
	UPROPERTY(VisibleAnywhere, Category = "Movement | Falling", meta = (AllowPrivateAccess = "true"))
	float FallingSpeed = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Movement | Falling", meta = (AllowPrivateAccess = "true"))
	bool IsFallingSoundPlayed = false;

	
	
	UPROPERTY(VisibleAnywhere, Category = "Movement | Falling", meta = (AllowPrivateAccess = "true"))
	TArray<FVector> CornerVector{};

	UPROPERTY(VisibleAnywhere, Category = "Movement | Falling", meta = (AllowPrivateAccess = "true"))
	float AccumulatedRotationTime = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Movement | Falling", meta = (AllowPrivateAccess = "true"))
	float SecondsToRotateFully = 5.f;
	
	UPROPERTY(VisibleAnywhere, Category = "Movement | Falling", meta = (AllowPrivateAccess = "true"))
	FQuat StartQuat{};

	UPROPERTY(VisibleAnywhere, Category = "Movement | Falling", meta = (AllowPrivateAccess = "true"))
	FQuat FinalQuat{};

	UPROPERTY(VisibleAnywhere, Category = "Movement | Falling", meta = (AllowPrivateAccess = "true"))
	int FirstUnsupportedCornerIndex = -1;
	
	UPROPERTY(VisibleAnywhere, Category = "Movement | Owner", meta = (AllowPrivateAccess = "true"))
	ABumperCar* OwnerPtr;

	UFUNCTION()
	FVector CalculateImpulseForces();

	UFUNCTION()
	void Move(float deltaTime);

	UFUNCTION()
	void CalculateSpeed(float deltaTime);

	UFUNCTION()
	bool IsGrounded() const;

	UFUNCTION()
	FVector GetLocationSnappedToGround(FVector nextPosition) const;

	UFUNCTION()
	int FindFirstUnsupportedCorner();

	UFUNCTION()
	void CheckForUnsupportedCorners();

	UFUNCTION()
	void RotateOnFloor();

	UFUNCTION()
	void RotateOnFalling(float deltaTime);

	UFUNCTION()
	void DisableMovementTimer(float deltaTime);

	UFUNCTION()
	void DisableAccelerationTimer(float deltaTime);

	UFUNCTION()
	void CalculateSpeedAcceleration(float deltaTime, float minimumSpeed, float maximumSpeed);
	
	UFUNCTION()
	void CalculateSpeedDeceleration(float deltaTime, float minimumSpeed, float maximumSpeed);

	//UFUNCTION()
	//void OnRep_ImpulseAdded();

	UFUNCTION()
	void OnRep_SetCurrentSpeed();
	
	UFUNCTION(Server, UnReliable)
	void Server_SetSpeed(float speed);

	UFUNCTION(NetMulticast, UnReliable)
	void Multicast_SetSpeed(float speed);
};
