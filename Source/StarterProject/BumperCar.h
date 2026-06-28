// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BumperCarComponent.h"
#include "BumperCar.generated.h"


struct FInputActionValue;
class UBoxComponent;
class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class UNiagaraComponent;
class USmoothSync;
class UBumperCarMovementComponent;
class UBounceOnHitComponent;
class UBumperCarState;
class UBumperCarComponent;
class UHealthComponent;
class UPoleMovementComponent;
class UAudioComponent;
class UBoostComponent;
class ATrail;
class UWidgetComponent;
class UNiagaraComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogBumperCar, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExtinguishPressEvent);

UCLASS()
class STARTERPROJECT_API ABumperCar : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABumperCar();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	FOnExtinguishPressEvent OnExtinguishPressEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsRespawning = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/////////////////////////
	//------MATERIALS------//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInstanceDynamic* CarMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInstanceDynamic* CharacterMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInstanceDynamic* GhostCarMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInstanceDynamic* GhostCharacterMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraComponent* SpeedBoostFXComp;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_NotifyRespawnReady();



	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:		
	/////////////////////////
	//------Input Functions------//
	UFUNCTION() void SteeringAction(const FInputActionValue& value);
	UFUNCTION() void CompletedSteeringAction();
	UFUNCTION() void AccelerateAction(const FInputActionValue& value);
	UFUNCTION() void CompletedAccelerating();
	UFUNCTION() void BrakeAction(const FInputActionValue& value);
	UFUNCTION() void CompletedBraking();
	UFUNCTION() void BoostAction();
	UFUNCTION() void CompletedBoostAction();
	UFUNCTION(Server, Reliable) void ServerExtinguishAction();
	UFUNCTION() void MoveCameraAction(const FInputActionValue& value);
	UFUNCTION() void ResetCameraMovement();
	UFUNCTION() void ResetBackWardsLookingCamera();
	UFUNCTION() void LookBackwards();

	UFUNCTION() void KillPlayer();

	UFUNCTION(Client, Reliable)
	void Client_OnDeathBoxDeath();

	UFUNCTION(Client, Reliable)
	void Client_OnRespawn();

	UFUNCTION(Client, Reliable)
	void Client_DeathEffect();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetCollisionProfile(FName ProfileName);
	UFUNCTION() void ChangeToGhostState();
	UFUNCTION() void ActivateBoostNetEffect(TArray<FVector> boostNetLightningPoints, bool isActive);

	UFUNCTION() void SetCarColor(float color);

	/////////////////////////
	//------VFX EVENTS------//
	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnGhostEvent(bool active);


	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void DeathEffect();

	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnDeathBoxDeathEvent();

	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnRespawnEvent();

	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnSlimedEvent(bool Activate);

	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnFireDeathEvent();

	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnFireEvent(bool Activate);

	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnFallingEvent(bool Activate);

	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnHitEvent(float intensity);

	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnChargedPoleEvent(float boostCharge, bool Activate);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnHighSpeedEvent(bool Activate);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnBumpEvent(bool Activate);

	UFUNCTION(BlueprintImplementableEvent, Category = "Stats")
	void OnRoundWonEvent(int numberOfWins);

	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnBoostNetEvent(const TArray<FVector>& lightningPoints, bool isActive);

	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnBoostActiveEvent(bool isActive);

	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnReversingEvent(bool isActive);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateDamageMaterialEffect();

	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnPlayerSpawn();

	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnPoleMovedEvent(FVector polePosition);

	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnExtinguishPressedEvent();

	UFUNCTION() void DisablePhysics();
	UFUNCTION(BlueprintCallable) void DisableMovement(bool disabled);
	UFUNCTION(BlueprintCallable) void OnRespawnFinished();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnGhostEvent(bool active);

	
	UFUNCTION(Client, Reliable)
	 void Client_IsSlimed(bool isSlimed);

	 UFUNCTION(Client, Reliable)
	 void Client_FadeIn();
	 UFUNCTION(BlueprintImplementableEvent) void FadeInEvent();


	/////////////////////////
	//------GETTERS------//
	UFUNCTION() ATrail* GetTrail() const { return TrailPtr; }
	UFUNCTION() UBoxComponent* GetCollisionBox() const { return CenterCollisionPtr; }
	UFUNCTION() UCapsuleComponent* GetCarCollider() const { return CarCollisionPtr; };
	UFUNCTION() UBumperCarMovementComponent* GetBumperCarMovementComponent() const { return MovementComponentPtr; }
	UFUNCTION() UHealthComponent* GetHealthComponent() const { return HealthComponentPtr; }
	UFUNCTION() UBounceOnHitComponent* GetOnHitComponent() const { return BounceOnHitComponentPtr; }
	UFUNCTION() EBumperCarStateType GetStateType() const { return BumperCarComponentPtr->GetStateType(); }
	UFUNCTION(BlueprintCallable) bool GetIsUnderBoostNet() const { return bUnderBoostNet; }
	
	UFUNCTION() void OnRoundWon(int wins);

	UFUNCTION(Server, Reliable)
	void Server_HandleDeathBox();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ResetBumperCar();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RespawnBumperCar(const FTransform& NewTransform);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_NewRoundRespawnBumperCar(const FTransform& NewTransform);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartFire();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopFire();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnHonkEffect();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBumperCarComponent> BumperCarComponentPtr;

	UPROPERTY(ReplicatedUsing = OnRep_CarColor, BlueprintReadWrite, Category = "Color")
	float CarColor = 0.0f;

	UPROPERTY()
	float LastDeathBoxTime = -999.f;

private:
	// COMPONENTS
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> CenterCollisionPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CarCollisionPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraPtr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> BumperCarMeshPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> FirstLightBulbPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SecondLightBulbPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ThirdLightBulbPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USmoothSync> SmoothSyncPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBumperCarMovementComponent> MovementComponentPtr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBounceOnHitComponent> BounceOnHitComponentPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAudioComponent> AudioComponentEnginePtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAudioComponent> AudioComponentBoostPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAudioComponent> AudioComponentHonkingPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoostComponent> BoostComponentPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthComponent> HealthComponentPtr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPoleMovementComponent> PoleMovementComponentPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trail", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ATrail> TrailPtr;

	UFUNCTION() void SetupPhysics();
	UFUNCTION() void CreateTrail();
	

	UFUNCTION()
	void OnRep_CarColor();

	// TRAIL PROPERTIES
	UPROPERTY(EditAnywhere, Category = "Trail", meta = (AllowPrivateAcces = "true"))
	float TrailLengthOffset = 225.f;

	UPROPERTY(EditAnywhere, Category = "Trail", meta = (AllowPrivateAcces = "true"))
	float TrailHeightOffset = 55.f;

	// GROUND COLLISION PROPERTIES
	UPROPERTY(VisibleAnywhere, Category = "Collision", meta = (AllowPrivateAcces = "true"))
	bool bIsGround = false;

	// CAMERA SETTINGS
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CurrentCameraFOV = 90.f;
	
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraDefaultFOV = 90.f;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraMaxFOV = 130.f;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	FRotator DefaultCameraRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	FRotator CurrentCameraRotation = FRotator::ZeroRotator;
	
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	FRotator DesiredCameraRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraRotationSpeed = 15.f;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	bool bIsLookingBackwards = false; 
	
	// BOOST NET SETTINGS
	UPROPERTY(VisibleAnywhere, Category = "BoostNet", meta = (AllowPrivateAcces = "true"))
	bool bUnderBoostNet = false;
	
	UPROPERTY(VisibleAnywhere, Category = "BoostNet", meta = (AllowPrivateAcces = "true"))
	TArray<FVector> BoostNetLightningPoints;
	
	UFUNCTION() void UpdateCameraFOV();

	//UPROPERTY(ReplicatedUsing=OnRoundWon)
	//int NumberOfWins = 0;
	
	UFUNCTION()	void ResetBumperCar();

	UFUNCTION() void RespawnBumperCar(FTransform transform);

	UFUNCTION() void MoveCamera(float deltaTime);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ActivateCarSound(bool isActivated);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnBoostNetEffect(const TArray<FVector>& lightningPoints, bool IsActive);
	//UFUNCTION() void RespawnBumperCar(FTransform transform);
};	
