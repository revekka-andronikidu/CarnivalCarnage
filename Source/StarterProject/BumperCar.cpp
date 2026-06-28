// Copyright Abonneer, Inc. All Rights Reserved.


#include "BumperCar.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "BoostComponent.h"
#include "SmoothSync.h"
#include "Trail.h"
#include "BoostNet.h"
#include "BumperCarMovementComponent.h"
#include "States/BumperCarGhostState.h"
#include "BounceOnHitComponent.h"
#include "BumperCarComponent.h"
#include "HealthComponent.h"
#include "PoleMovementComponent.h"
#include "Bumper.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "States/BumperCarOnFireState.h"
#include "Components/WidgetComponent.h"
#include "StarterProject/GameModes/GameplayGameMode.h"
#include "NiagaraSystem.h"

// niagara
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Input
#include "EnhancedInputSubsystems.h"
#include "StarterProject/PlayerController/MainPlayerController.h"

// networking
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogBumperCar);

// Sets default values
ABumperCar::ABumperCar()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	

	CenterCollisionPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("CenterCollision"));
	CenterCollisionPtr->InitBoxExtent(FVector{ 195.f, 125.f, 32.f });
	RootComponent = CenterCollisionPtr;

	CarCollisionPtr = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionBox"));
	CarCollisionPtr->SetupAttachment(RootComponent);
	CarCollisionPtr->SetRelativeLocation(FVector{ 0.f, 0.f, 130.f });
	CarCollisionPtr->SetRelativeRotation(FRotator{ 90.f, 0.f, 0.f });
	CarCollisionPtr->SetCapsuleHalfHeight(210.f);
	CarCollisionPtr->SetCapsuleRadius(125.f);
	CarCollisionPtr->ComponentTags.Add(TEXT("CarCollision"));

	BumperCarMeshPtr = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BumperCarMesh"));
	BumperCarMeshPtr->SetupAttachment(RootComponent);
	BumperCarMeshPtr->SetRelativeLocation(FVector{ 0.f, 0.f, -30.f });

	FirstLightBulbPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightBulb_1"));
	FirstLightBulbPtr->SetupAttachment(BumperCarMeshPtr);
	FirstLightBulbPtr->SetRelativeLocation(FVector{ -96.75f, -56.32f, 194.63f });
	FirstLightBulbPtr->SetRelativeRotation(FRotator{ 0.32f, 12.70f, -43.94f });

	SecondLightBulbPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightBulb_2"));
	SecondLightBulbPtr->SetupAttachment(BumperCarMeshPtr);
	SecondLightBulbPtr->SetRelativeLocation(FVector{ -101.69f, -30.75f, 206.40f });
	SecondLightBulbPtr->SetRelativeRotation(FRotator{ -3.96f, 0.0f, -12.16f });

	ThirdLightBulbPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightBulb_3"));
	ThirdLightBulbPtr->SetupAttachment(BumperCarMeshPtr);
	ThirdLightBulbPtr->SetRelativeLocation(FVector{ -105.48f, 0.0f, 211.42f });

	SpringArmPtr = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmPtr->SetupAttachment(RootComponent);
	SpringArmPtr->SetRelativeLocation(FVector{ 0.f, 0.f, 50.f });
	SpringArmPtr->TargetArmLength = 300.f;
	SpringArmPtr->SocketOffset.Z = 150.f;
	SpringArmPtr->bDoCollisionTest = true;
	SpringArmPtr->ProbeChannel = ECC_GameTraceChannel1;
	SpringArmPtr->bUsePawnControlRotation = false;
	SpringArmPtr->bInheritRoll = false;
	SpringArmPtr->bInheritYaw = true;
	SpringArmPtr->bInheritPitch = false;
	SpringArmPtr->bEnableCameraLag = true;
	SpringArmPtr->bEnableCameraRotationLag = true;
	SpringArmPtr->CameraLagSpeed = 7.5f;
	SpringArmPtr->CameraRotationLagSpeed = 7.5f;
	SpringArmPtr->CameraLagMaxDistance = 50.f;

	CameraPtr = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraPtr->SetupAttachment(SpringArmPtr);
	CameraPtr->SetRelativeRotation(FRotator{ 0.f, -10.f, 0.f });
	CameraPtr->bAutoActivate = true;
	CameraPtr->SetRelativeRotation(FQuat::MakeFromEuler(FVector{ 0.f, 0.f, 0.f }));
	CameraPtr->SetFieldOfView(90.f);

	AudioComponentEnginePtr = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponentEngine"));
	AudioComponentEnginePtr->SetupAttachment(RootComponent);
	AudioComponentEnginePtr->bAutoActivate = false;

	AudioComponentBoostPtr = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponentBoost"));
	AudioComponentBoostPtr->SetupAttachment(RootComponent);
	AudioComponentBoostPtr->bAutoActivate = false;

	AudioComponentHonkingPtr = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponentHonking"));
	AudioComponentHonkingPtr->SetupAttachment(RootComponent);
	AudioComponentHonkingPtr->bAutoActivate = false;

	// CUSTOM COMPONENTS
	MovementComponentPtr = CreateDefaultSubobject<UBumperCarMovementComponent>(TEXT("MovementComponent"));
	BounceOnHitComponentPtr = CreateDefaultSubobject<UBounceOnHitComponent>(TEXT("BounceComponent"));
	BumperCarComponentPtr = CreateDefaultSubobject<UBumperCarComponent>(TEXT("BumperCarComponent"));
	HealthComponentPtr = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	BoostComponentPtr = CreateDefaultSubobject<UBoostComponent>(TEXT("BoostComponent"));
	PoleMovementComponentPtr = CreateDefaultSubobject<UPoleMovementComponent>(TEXT("PoleMovementComponent"));

	// REPLICATION
	SmoothSyncPtr = CreateDefaultSubobject<USmoothSync>(TEXT("SmoothSync"));
	SetReplicateMovement(true);

	bReplicates = true;
	bAlwaysRelevant = true;
	SetNetCullDistanceSquared(FLT_MAX);
	NetDormancy = ENetDormancy::DORM_Never;

	Tags.Add(TEXT("BumperCar"));
}

// Called when the game starts or when spawned
void ABumperCar::BeginPlay()
{
	Super::BeginPlay();

	SetupPhysics();

	if (HealthComponentPtr)
	{
		HealthComponentPtr->SetupHealthComponent(100);
	}

	if (CarCollisionPtr)
	{
		CarCollisionPtr->OnComponentBeginOverlap.AddDynamic(this, &ABumperCar::OnOverlapBegin);
	}

	TArray<UActorComponent*> Components;
	GetComponents(UNiagaraComponent::StaticClass(), Components);

	for (UActorComponent* Comp : Components)
	{
		if (Comp && Comp->GetName() == TEXT("SpeedBoostFX"))
		{
			
			SpeedBoostFXComp = Cast<UNiagaraComponent>(Comp);
			break;
		}
	}
	if (!SpeedBoostFXComp)
	{
		UE_LOG(LogBumperCar, Log, TEXT("Niagara SpeedBoostFX NOT found"));
	}

	PoleMovementComponentPtr->SetupComponent(MovementComponentPtr, this);
	CreateTrail();
	Multicast_ActivateCarSound(true);
}

void ABumperCar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate CarColor to clients
	DOREPLIFETIME(ABumperCar, CarColor);
}

void ABumperCar::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// hit with car
	if (!HasAuthority()) { return; }

	if (!SweepResult.GetActor()) { return; }

	if (OtherComp->ComponentHasTag("CarCollision") and
		SweepResult.GetActor() != this)
	{
		BounceOnHitComponentPtr->BounceOnCarHit(SweepResult);
		BoostComponentPtr->Multicast_EmptyBoost();
	}

	// hit bumper obstacle
	if (SweepResult.GetActor()->IsA(ABumper::StaticClass()))
	{
		BounceOnHitComponentPtr->BounceOnBumperHit(SweepResult);
		BoostComponentPtr->Multicast_EmptyBoost();
	}

	if (SweepResult.GetActor()->ActorHasTag(FName{ "StaticBoundary" }))
	{
		BounceOnHitComponentPtr->BounceOnStaticObject(SweepResult);
	}
}

void ABumperCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCameraFOV();
	if (bUnderBoostNet) { Multicast_OnBoostNetEffect(BoostNetLightningPoints, bUnderBoostNet); }

	if (CurrentCameraRotation != DesiredCameraRotation) { MoveCamera(DeltaTime); }
}

void ABumperCar::SteeringAction(const FInputActionValue& value)
{
	FVector inputValue = value.Get<FVector>();
	MovementComponentPtr->Steer(inputValue.X);
}

void ABumperCar::CompletedSteeringAction()
{
	MovementComponentPtr->Steer(0.f);
}

void ABumperCar::AccelerateAction(const FInputActionValue& value)
{
	FVector inputValue = value.Get<FVector>();
	if (inputValue.X > 0.1f)
	{
		GEngine->AddOnScreenDebugMessage(0, 2.f, FColor::Cyan, TEXT("Accelerating"));
		MovementComponentPtr->Accelerate(inputValue.X, false);

	}

}

void ABumperCar::CompletedAccelerating()
{
	MovementComponentPtr->Accelerate(0.f, true);
}

void ABumperCar::BrakeAction(const FInputActionValue& value)
{
	FVector inputValue = value.Get<FVector>();
	if (inputValue.X > 0.1f)
	{
		//GEngine->AddOnScreenDebugMessage(0, 2.f, FColor::Cyan, TEXT("Braking"));
		MovementComponentPtr->Brake(inputValue.X, false);
	}
}

void ABumperCar::CompletedBraking()
{
	MovementComponentPtr->Brake(0.f, true);
}

void ABumperCar::ServerExtinguishAction_Implementation()
{
	FString StateString = UEnum::GetValueAsString(BumperCarComponentPtr->GetStateType());
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
		FString::Printf(TEXT("BumperCar State: %s"), *StateString));


	//if (BumperCarComponentPtr->GetStateType() == EBumperCarStateType::OnFire)
	//{
		UBumperCarOnFireState* OnFireState = Cast<UBumperCarOnFireState>(BumperCarComponentPtr->GetState());
		if (OnFireState)
		{
			UE_LOG(LogTemp, Warning, TEXT("ExtinguishAction pressed"));
			OnFireState->ExtinguishButtonPressed();
		}
	//}
}

void ABumperCar::BoostAction()
{
	BoostComponentPtr->Boost(false);
	AudioComponentBoostPtr->Stop();
	//OnBoostActiveEvent(true);
}

void ABumperCar::CompletedBoostAction()
{
	BoostComponentPtr->Boost(true);
	AudioComponentBoostPtr->Play();
	OnBoostActiveEvent(false);
}

void ABumperCar::MoveCameraAction(const FInputActionValue& value)
{
	if (bIsLookingBackwards) { return; }

	FVector input = value.Get<FVector>();

	float CameraYaw = FMath::Atan2(input.X, input.Y) * (180.f / PI) - 180.f;
	CameraYaw = FMath::UnwindDegrees(CameraYaw);
	DesiredCameraRotation = FRotator{ 0.f, CameraYaw, 0.f };
}

void ABumperCar::ResetCameraMovement()
{
	// go back to normal rotation
	if (bIsLookingBackwards) { return; }

	DesiredCameraRotation = DefaultCameraRotation;
	bIsLookingBackwards = false;
}

void ABumperCar::ResetBackWardsLookingCamera()
{
	if (!bIsLookingBackwards) { return; }

	DesiredCameraRotation = DefaultCameraRotation;
	bIsLookingBackwards = false;
}

void ABumperCar::LookBackwards()
{
	if (bIsLookingBackwards) { return; }

	DesiredCameraRotation = FRotator{ 0.f, 180.f, 0.f };
	bIsLookingBackwards = true;
}

void ABumperCar::ActivateBoostNetEffect(TArray<FVector> boostNetLightningPoints, bool isActive)
{
	if (bUnderBoostNet != isActive) 
	{
		if (bUnderBoostNet)
		{
			bUnderBoostNet = isActive;
			Multicast_OnBoostNetEffect(boostNetLightningPoints, isActive);
		}
		else
		{
			bUnderBoostNet = isActive;
		}
	}
	bUnderBoostNet = isActive;
	BoostNetLightningPoints = boostNetLightningPoints;
}

void ABumperCar::DisablePhysics()
{
	CenterCollisionPtr->SetSimulatePhysics(false);
	CenterCollisionPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CenterCollisionPtr->SetNotifyRigidBodyCollision(false);
	CenterCollisionPtr->SetCollisionResponseToAllChannels(ECR_Overlap);
	CenterCollisionPtr->SetGenerateOverlapEvents(false);

	BumperCarMeshPtr->SetHiddenInGame(true);
	TrailPtr->SetHidden(true);
	MovementComponentPtr->DisableSteering(true);
}

void ABumperCar::DisableMovement(bool disabled)
{
	MovementComponentPtr->DisableMovement(disabled);
}

void ABumperCar::SetupPhysics()
{
	CenterCollisionPtr->SetSimulatePhysics(false);
	CenterCollisionPtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CenterCollisionPtr->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	CenterCollisionPtr->SetNotifyRigidBodyCollision(true);
	CenterCollisionPtr->SetCollisionResponseToAllChannels(ECR_Overlap);
	CenterCollisionPtr->SetGenerateOverlapEvents(true);
}

void ABumperCar::CreateTrail()
{
	if (!HasAuthority()) { return; }

	FActorSpawnParameters spawnParameters{};
	spawnParameters.Owner = this;
	FVector location = GetActorLocation();
	location -= TrailLengthOffset * GetActorForwardVector();
	location.Z += TrailHeightOffset;
	
	TrailPtr = GetWorld()->SpawnActor<ATrail>(ATrail::StaticClass(), location, GetActorRotation(), spawnParameters);
	if (TrailPtr)
	{
		TrailPtr->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		TrailPtr->Multicast_SetHueShift(CarColor);
		UE_LOG(LogBumperCar, Log, TEXT("%s created %s"), *GetName(), *TrailPtr->GetName());
	}
	
}

void ABumperCar::UpdateCameraFOV()
{
	if (MovementComponentPtr->GetCurrentSpeed() < 0.f) { return; }

	float percentage = MovementComponentPtr->GetCurrentSpeed() / MovementComponentPtr->GetMaxBoostSpeed();
	CurrentCameraFOV = FMath::Lerp(CameraDefaultFOV, CameraMaxFOV, percentage);
	CameraPtr->SetFieldOfView(CurrentCameraFOV);
}

void ABumperCar::MoveCamera(float deltaTime)
{
	if (!MovementComponentPtr->GetMovementEnabled()) { return; }

	CurrentCameraRotation = FMath::RInterpTo(CurrentCameraRotation, DesiredCameraRotation, deltaTime, CameraRotationSpeed);
	SpringArmPtr->SetRelativeRotation(CurrentCameraRotation);
}

void ABumperCar::Multicast_OnHonkEffect_Implementation()
{
	AudioComponentHonkingPtr->Play();
}

void ABumperCar::Multicast_OnBoostNetEffect_Implementation(const TArray<FVector>& lightningPoints, bool IsActive)
{
	OnBoostNetEvent(lightningPoints, IsActive);
}

void ABumperCar::SetCarColor(float color)
{
	CarColor = color;
	OnRep_CarColor();
}

void ABumperCar::OnRep_CarColor()
{
	if (CarMaterialInstance)
	{
		CarMaterialInstance->SetScalarParameterValue("Hueshift", CarColor);
	}
	if (CharacterMaterialInstance)
	{
		CharacterMaterialInstance->SetScalarParameterValue("Player_color", CarColor);
	}
	if (GhostCharacterMaterialInstance)
	{
		GhostCharacterMaterialInstance->SetScalarParameterValue("Hue_Shift", CarColor);
	}
	if (GhostCarMaterialInstance)
	{
		GhostCarMaterialInstance->SetScalarParameterValue("Hue_Shift", CarColor);
	}
	if (SpeedBoostFXComp)
	{
		SpeedBoostFXComp->SetVariableFloat(FName("PlayerColor"), CarColor);
	}
	if (TrailPtr)
	{
		TrailPtr->Multicast_SetHueShift(CarColor);
	}
}

void ABumperCar::ResetBumperCar()
{
	HealthComponentPtr->ResetComponent();
	HealthComponentPtr->SetupHealthComponent(100);

	BumperCarComponentPtr->ResetComponent();
	MovementComponentPtr->ResetComponent();
	BoostComponentPtr->ResetComponent();
	PoleMovementComponentPtr->ResetComponent();

	ResetCameraMovement();
	ResetBackWardsLookingCamera();
	CameraPtr->SetFieldOfView(CameraDefaultFOV);

	this->OnFireEvent(false);

	if (TrailPtr)
	{
		TrailPtr->ResetTrail();
		TrailPtr->Multicast_SetHueShift(CarColor);
	}

	this->Multicast_SetCollisionProfile(FName("Car_Alive"));
	this->SetActorEnableCollision(true);

	//test
	MovementComponentPtr->DisableMovement(false);
}

void ABumperCar::OnRoundWon(int wins)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::FromInt(wins));

	// TODO: winner stuff
	OnRoundWonEvent(wins);
}

void ABumperCar::Multicast_RespawnBumperCar_Implementation(const FTransform& NewTransform)
{
	RespawnBumperCar(NewTransform);
	//OnPlayerSpawn();
}

void ABumperCar::Multicast_NewRoundRespawnBumperCar_Implementation(const FTransform& NewTransform)
{
	RespawnBumperCar(NewTransform);

}

void ABumperCar::Multicast_ResetBumperCar_Implementation()
{
	ResetBumperCar();
}

void ABumperCar::RespawnBumperCar(FTransform transform)
{
	UE_LOG(LogBumperCar, Warning, TEXT("Respawned car: %s"), *this->GetName());

	SetActorTransform(transform);

	MovementComponentPtr->ResetComponent();
	MovementComponentPtr->DisableMovement(false);

}

void ABumperCar::Server_HandleDeathBox_Implementation()
{
	if (!HasAuthority())
		return;

	//stop falling
	//disable colllison
	//on respawn enable


	MovementComponentPtr->DisableMovement(true);
	AGameplayGameMode* GM = Cast<AGameplayGameMode>(GetWorld()->GetAuthGameMode());

	if (HealthComponentPtr)
	{
		if (HealthComponentPtr && HealthComponentPtr->GetIsDead())
		{
			UE_LOG(LogBumperCar, Warning, TEXT("Dead player hit the DeathBox. Name: %s"), *GetName());
			//Server respawn as ghost

				if (GM)
				{
					//GM->RespawnAsGhost(this);
					
				}

				Client_OnDeathBoxDeath();
		}
		else
		{
			UE_LOG(LogBumperCar, Warning, TEXT("Alive player hit the DeathBoxName: %s"), *GetName());

			if (GM)
			{
				GM->KillPlayer(this);
			}
		}
	}
}

void ABumperCar::MulticastStartFire_Implementation()
{
	OnFireEvent(true);  // toggle VFX/sounds
}

void ABumperCar::MulticastStopFire_Implementation()
{
	OnFireEvent(false); // toggle VFX/sounds
}

void ABumperCar::Multicast_ActivateCarSound_Implementation(bool isActivated)
{
	if (AudioComponentEnginePtr)
	{
		if (isActivated)
		{
			AudioComponentEnginePtr->Play();
		}
		else
		{
			AudioComponentEnginePtr->Stop();
		}
	}
}

void ABumperCar::Multicast_OnGhostEvent_Implementation(bool active)
{
	OnGhostEvent(active); // Blueprint event, runs on all machines
}

void ABumperCar::ChangeToGhostState()
{
	BumperCarComponentPtr->SetState(UBumperCarGhostState::StaticClass());
}

void ABumperCar::Client_OnDeathBoxDeath_Implementation()
{
	if (!IsLocallyControlled())
		return;

	MovementComponentPtr->DisableMovement(true);
	AMainPlayerController* PC = Cast<AMainPlayerController>(GetController());
	if (PC)
		PC->GameComponent->Client_DisableGameInput();
	

	OnDeathBoxDeathEvent();
}

void ABumperCar::Client_DeathEffect_Implementation()
{
	if (!IsLocallyControlled())
		return;

	MovementComponentPtr->DisableMovement(true);
	//disable collision?

	DeathEffect();
}

void ABumperCar::Client_OnRespawn_Implementation()
{
	//if (!IsLocallyControlled())
	//	return;

	//MovementComponentPtr->DisableMovement(false);

	OnRespawnEvent();
}


void ABumperCar::Client_FadeIn_Implementation()
{
	//if (!IsLocallyControlled())
	//	return;

	//MovementComponentPtr->DisableMovement(false);

	FadeInEvent();
	IsRespawning = false;
}

void ABumperCar::OnRespawnFinished()
{
	IsRespawning = false;

	AMainPlayerController* PC = Cast<AMainPlayerController>(GetController());
	if (PC)
		PC->GameComponent->Client_OnMatchStart();

}

void ABumperCar::Multicast_SetCollisionProfile_Implementation(FName ProfileName)
{
	UE_LOG(LogBumperCar, Warning, TEXT("Collision profile set to: %s"), *ProfileName.ToString());


	if (GetCollisionBox())
		GetCollisionBox()->SetCollisionProfileName(ProfileName);

	if (GetCarCollider())
		GetCarCollider()->SetCollisionProfileName(ProfileName);
}

void ABumperCar::KillPlayer()
{
	HealthComponentPtr->Server_KillOwner();
}

void ABumperCar::Server_NotifyRespawnReady_Implementation()
{
	if (!HasAuthority()) return;

	AGameplayGameMode* GM = Cast<AGameplayGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM) return;

	GM->RequestedRespawn(this);
}

void ABumperCar::Client_IsSlimed_Implementation(bool isSlimed)
{
	OnSlimedEvent(isSlimed);
}