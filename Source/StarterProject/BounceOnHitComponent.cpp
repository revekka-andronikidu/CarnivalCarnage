// Copyright Abonneer, Inc. All Rights Reserved.


#include "BounceOnHitComponent.h"
#include "Trail.h"
#include "Bumper.h"
#include "BumperCar.h"
#include "Components/BoxComponent.h"
#include "BumperCarMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Templates/Tuple.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/MainPlayerState.h"

// niagara
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

DEFINE_LOG_CATEGORY(LogBounceOnHitComponent);

// Sets default values for this component's properties
UBounceOnHitComponent::UBounceOnHitComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBounceOnHitComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBounceOnHitComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (GetOwner()->GetWorldTimerManager().IsTimerActive(HitTimerHandle)) { return; }
	
	if (bIsHit)
	{
		GetOwner()->GetWorldTimerManager().SetTimer(
			HitTimerHandle,
			this,
			&UBounceOnHitComponent::TurnOffTimer,
			0.2f,
			false
		);
	}

	if (!BumpEffectPtr)
	{
		BumpEffectPtr = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/ArtAssets/Effects/N_Bump_System.N_Bump_System"));
	}
	if (!ImpactSoundPtr)
	{
		ImpactSoundPtr = LoadObject<USoundBase>(nullptr, TEXT("/Script/Engine.SoundWave'/Game/ArtAssets/Sounds/Effects/AFX_Impact_Sounds_1.AFX_Impact_Sounds_1'"));
	}
}

void UBounceOnHitComponent::BounceOnCarHit(const FHitResult& Hit)
{
	Multicast_BounceOnCarHit(Hit);
	
	auto* car = Cast<ABumperCar>(GetOwner());
	if (!car) { return; }
	
	if (ATrail* trail = Cast<ATrail>(car->GetTrail()))
	{
		trail->OnHit();
	}
}

void UBounceOnHitComponent::BounceOnBumperHit(const FHitResult& hit)
{
	if (ABumper* bumper = Cast<ABumper>(hit.GetActor()))
	{
		bumper->NetMulticast_PlaySoundCue();
	}

	Multicast_BounceOnBumperHit(hit);

	auto* car = Cast<ABumperCar>(GetOwner());
	if (!car) { return; }

	if (ATrail* trail = Cast<ATrail>(car->GetTrail()))
	{
		trail->OnHit();
	}
}

void UBounceOnHitComponent::BounceOnStaticObject(const FHitResult& hit)
{
	Multicast_BounceOnStaticObject(hit);

	auto* car = Cast<ABumperCar>(GetOwner());
	if (!car) { return; }

	if (ATrail* trail = Cast<ATrail>(car->GetTrail()))
	{
		trail->OnHit();
	}
}

void UBounceOnHitComponent::BounceOnTrapdoorDoor(const FHitResult& hit)
{
	Multicast_BounceOnTrapDoorObject(hit);

	auto* car = Cast<ABumperCar>(GetOwner());
	if (!car) { return; }

	if (ATrail* trail = Cast<ATrail>(car->GetTrail()))
	{
		trail->OnHit();
	}
}

void UBounceOnHitComponent::Multicast_BounceOnCarHit_Implementation(const FHitResult& hit)
{
	if (bIsHit) { return; }

	auto* car = Cast<ABumperCar>(GetOwner());
	auto* otherCar = Cast<ABumperCar>(hit.GetActor());

	if (!car or !otherCar) { return; }

	// calculates hit normal and force multipliers
	FVector hitNormal = CalculateCarHitNormal(car, otherCar);
	TPair<float, float> forces = CalculateCarHitForceMultiplier(car, otherCar);

	// applies impulse forces to both cars
	UE_LOG(LogBounceOnHitComponent, Log, TEXT("%s force = %s"), *car->GetName(), *(hitNormal * forces.Key * CarHitForce).ToString());
	car->GetBumperCarMovementComponent()->Impulse(hitNormal * forces.Key * CarHitForce);
	UE_LOG(LogBounceOnHitComponent, Log, TEXT("%s force = %s"), *otherCar->GetName(), *(hitNormal * forces.Value * CarHitForce).ToString());
	otherCar->GetBumperCarMovementComponent()->Impulse(-hitNormal * forces.Value * CarHitForce);
	
	// does controller rumble and camera shake
	car->OnHitEvent(forces.Key);
	car->CompletedBoostAction();
	otherCar->OnHitEvent(forces.Value);
	otherCar->CompletedBoostAction();

	// play visual effect
	FVector effectLocation = CalculateHitLocation(car, otherCar);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(car->GetWorld(), BumpEffectPtr, effectLocation);

	// play sound
	UGameplayStatics::PlaySoundAtLocation(car->GetWorld(), ImpactSoundPtr, effectLocation);
	car->Multicast_OnHonkEffect();

	bIsHit = true;
	otherCar->GetOnHitComponent()->SetIsHit(true);

	UE_LOG(LogBounceOnHitComponent, Log, TEXT("%s, has hit other car %s"), *car->GetName(), *otherCar->GetName());
}

void UBounceOnHitComponent::Multicast_BounceOnBumperHit_Implementation(const FHitResult& hit)
{
	auto* car = Cast<ABumperCar>(GetOwner());
	auto* bumper = Cast<ABumper>(hit.GetActor());

	if (!car or !bumper) { return; }

	// calculate force
	float forcePercentage = CalculateBumperHitForcePercentage(car);
	float force = forcePercentage * HitForceOnBumper;
	UE_LOG(LogBounceOnHitComponent, Log, TEXT("%s force = %f"), *car->GetName(), force);
	
	// apply impulse
	FVector hitNormal = CalculateBasicHitNormal(car, bumper);
	car->GetBumperCarMovementComponent()->Impulse(hitNormal * force);
	
	// play camera shake and controller rumble
	car->OnHitEvent(FMath::Lerp(1.f, 4.f, forcePercentage));
	car->CompletedBoostAction();

	// play visual effect
	FVector effectLocation = CalculateHitLocation(car, bumper);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(car->GetWorld(), BumpEffectPtr, effectLocation);

	bIsHit = true;
	UE_LOG(LogBounceOnHitComponent, Log, TEXT("%s has hit bumper %s"), *car->GetName(), *bumper->GetName());
}

void UBounceOnHitComponent::Multicast_BounceOnStaticObject_Implementation(const FHitResult& hit)
{
	auto* car = Cast<ABumperCar>(GetOwner());
	auto* staticObject = hit.GetActor();

	if (!car or !staticObject) { return; }

	// apply impulse
	FVector hitNormal = CalculateBasicHitNormal(car, staticObject);
	car->GetBumperCarMovementComponent()->Impulse(hitNormal * HitForceOnStaticObject);
	
	// play camera shake and controller rumble
	car->OnHitEvent(1.f);
	car->CompletedBoostAction();

	// play visual effect
	FVector effectLocation = CalculateHitLocation(car, staticObject);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(car->GetWorld(), BumpEffectPtr, effectLocation);
	
	
	bIsHit = true;
	UE_LOG(LogBounceOnHitComponent, Log, TEXT("%s has hit static object %s"), *car->GetName(), *staticObject->GetName());
}

void UBounceOnHitComponent::Multicast_BounceOnTrapDoorObject_Implementation(const FHitResult& hit)
{
	auto* car = Cast<ABumperCar>(GetOwner());
	auto* staticObject = hit.GetActor();

	if (!car or !staticObject) { return; }

	FVector hitNormal = CalculateBasicHitNormal(car, staticObject);
	hitNormal.Z = 0.f;
	car->GetBumperCarMovementComponent()->Impulse(hitNormal * HitForceOnTrapdoorDoor);

	bIsHit = true;
	UE_LOG(LogBounceOnHitComponent, Log, TEXT("%s has hit static object %s"), *car->GetName(), *staticObject->GetName());
}

void UBounceOnHitComponent::TurnOffTimer()
{
	bIsHit = false;
	GetOwner()->GetWorldTimerManager().ClearTimer(HitTimerHandle);
}

TPair<float, float>  UBounceOnHitComponent::CalculateCarHitForceMultiplier(ABumperCar* bumperCar, ABumperCar* otherCar) const
{
	float maxSpeed = bumperCar->GetBumperCarMovementComponent()->GetMaxBoostSpeed();
	float bumperCarSpeed = FMath::Abs(bumperCar->GetBumperCarMovementComponent()->GetCurrentSpeed());
	float otherCarSpeed = FMath::Abs(otherCar->GetBumperCarMovementComponent()->GetCurrentSpeed());
	UE_LOG(LogBounceOnHitComponent, Log, TEXT("max speed = %f"), maxSpeed);
	UE_LOG(LogBounceOnHitComponent, Log, TEXT("%s's speed = %f"), *bumperCar->GetName(), bumperCarSpeed);
	UE_LOG(LogBounceOnHitComponent, Log, TEXT("%s's speed = %f"), *otherCar->GetName(), otherCarSpeed);

	float intensity = FMath::Clamp((FMath::Abs(bumperCarSpeed) + FMath::Abs(otherCarSpeed)) / (2 * maxSpeed), 0.f, 1.f);
	UE_LOG(LogBounceOnHitComponent, Log, TEXT("intensity = %f"), intensity);
	float normalizedBumperCarSpeed = FMath::Abs(bumperCarSpeed) / maxSpeed;
	UE_LOG(LogBounceOnHitComponent, Log, TEXT("%s's Normalized Speed = %f"), *bumperCar->GetName(), normalizedBumperCarSpeed);
	float normalizedOtherCarSpeed = FMath::Abs(otherCarSpeed) / maxSpeed;
	UE_LOG(LogBounceOnHitComponent, Log, TEXT("%s's Normalized Speed = %f"), *otherCar->GetName(), normalizedOtherCarSpeed);
	
	float forceBumperCar = (1 + 3 * (otherCarSpeed / (bumperCarSpeed + otherCarSpeed)) * intensity);
	UE_LOG(LogBounceOnHitComponent, Log, TEXT("%s's Force multiplier = %f"), *bumperCar->GetName(), forceBumperCar);
	float forceOtherCar = (1 + 3 * (bumperCarSpeed / (bumperCarSpeed + otherCarSpeed)) * intensity);
	UE_LOG(LogBounceOnHitComponent, Log, TEXT("%s's Force multiplier = %f"), *otherCar->GetName(), forceOtherCar);

	return TPair<float, float>(forceBumperCar, forceOtherCar);
}

float UBounceOnHitComponent::CalculateBumperHitForcePercentage(ABumperCar* bumperCar) const
{
	float speed = bumperCar->GetBumperCarMovementComponent()->GetCurrentSpeed();
	float maxSpeed = bumperCar->GetBumperCarMovementComponent()->GetMaxBoostSpeed();

	float percentage = FMath::Clamp(FMath::Abs(speed) / maxSpeed, 0.25f, 1.f);
	return percentage;
}

FVector UBounceOnHitComponent::CalculateBasicHitNormal(AActor* actor, AActor* otherActor) const
{
	FVector firstActorLocation = actor->GetActorLocation();
	FVector secondActorLocation = otherActor->GetActorLocation();
	if (actor->IsA(ABumperCar::StaticClass()))
	{
		firstActorLocation.Z += 50.f;
	}
	if (otherActor->IsA(ABumperCar::StaticClass()))
	{
		secondActorLocation.Z += 50.f;
	}
	FVector positionNormal = (firstActorLocation - secondActorLocation).GetSafeNormal();
	return positionNormal;
}

FVector UBounceOnHitComponent::CalculateCarHitNormal(ABumperCar* bumperCar, ABumperCar* otherCar) const
{
	if (otherCar)
	{
		FVector positionNormal = (bumperCar->GetActorLocation() - otherCar->GetActorLocation()).GetSafeNormal();

		FVector hitNormal = positionNormal.GetSafeNormal();
		UE_LOG(LogBounceOnHitComponent, Log, TEXT("hitnormal: %s"), *hitNormal.ToString());
		return hitNormal;
	}
	UE_LOG(LogBounceOnHitComponent, Error, TEXT("Failed to calculate hitnormal, otherCar not valid"));
	return FVector::ZeroVector;
}

FVector UBounceOnHitComponent::CalculateHitLocation(AActor* actor, AActor* otherActor) const
{
	FVector hitLocation = (actor->GetActorLocation() + otherActor->GetActorLocation()) / 2.f;
	hitLocation.Z += 100.f;

	return hitLocation;
}
