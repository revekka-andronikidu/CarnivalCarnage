// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrailCollisionComponent.generated.h"

class ABumperCar;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFireTrailOverlapSignature, AActor*, overlappingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlimeTrailOverlapSignature, AActor*, overlappingActor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STARTERPROJECT_API UTrailCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTrailCollisionComponent();

	UPROPERTY()
	FOnFireTrailOverlapSignature OnFireTrailOverlapEvent;

	UPROPERTY()
	FOnSlimeTrailOverlapSignature OnSlimeTrailOverlapEvent;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY()
	ABumperCar* OwnerOfTrail;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnTrailHit(ABumperCar* hitCar, bool isDead);
};
