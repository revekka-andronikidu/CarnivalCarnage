#include "TrailCollisionComponent.h"
// Copyright Abonneer, Inc. All Rights Reserved.


#include "TrailCollisionComponent.h"
#include "Trail.h"
#include "BumperCar.h"
#include "BumperCarMovementComponent.h"
#include "HealthComponent.h"
#include "components/BoxComponent.h"

// Sets default values for this component's properties
UTrailCollisionComponent::UTrailCollisionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UTrailCollisionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerOfTrail = Cast<ABumperCar>(GetOwner()->GetOwner());
}


// Called every frame
void UTrailCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTrailCollisionComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (!OtherComp->ComponentHasTag("CarCollision")) { return; }

	if (auto* bumperCar = Cast<ABumperCar>(OtherActor))
	{
		if (OwnerOfTrail != bumperCar)
		{
			Multicast_OnTrailHit(bumperCar, OwnerOfTrail->GetHealthComponent()->GetIsDead());
		}
		else if (ATrail* trail = Cast<ATrail>(GetOwner()))
		{
			if (trail->GetOwnerIsHit())
			{
				UE_LOG(LogTrailActor, Log, TEXT("%s Owner Of trail Hit trail %s owned by %s"), *bumperCar->GetController()->GetName(), *OverlappedComp->GetName(), *OverlappedComp->GetOwner()->GetName());
				trail->DeleteCollisionOnHit(OverlappedComp);
				return;
			}
		}
	}
}

void UTrailCollisionComponent::Multicast_OnTrailHit_Implementation(ABumperCar* hitCar, bool isDead)
{
	if (isDead)
	{
		OnSlimeTrailOverlapEvent.Broadcast(hitCar);
		hitCar->GetBumperCarMovementComponent()->IsSlimed(1.5f);
	}
	else
	{
		OnFireTrailOverlapEvent.Broadcast(hitCar);
		//GEngine->AddOnScreenDebugMessage(4, 2.f, FColor::Emerald, TEXT("ON FIRE"));
	}
}

