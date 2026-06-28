// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BounceOnHitComponent.generated.h"

class UBumperCarMovementComponent;
class ABumperCar;
class UNiagaraSystem;
class UNiagaraComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogBounceOnHitComponent, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STARTERPROJECT_API UBounceOnHitComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBounceOnHitComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void BounceOnCarHit(const FHitResult& Hit);

	UFUNCTION()
	void BounceOnBumperHit(const FHitResult& hit);

	UFUNCTION()
	void BounceOnStaticObject(const FHitResult& hit);

	UFUNCTION()
	void BounceOnTrapdoorDoor(const FHitResult& hit);

	UFUNCTION() bool GetIsHit() const { return bIsHit; }
	UFUNCTION() void SetIsHit(bool isHit) { bIsHit = isHit; }
	UFUNCTION() void TurnOffTimer();

private:
	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* BumpEffectPtr;
	
	UPROPERTY(VisibleAnywhere)
	USoundBase* ImpactSoundPtr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Force", meta = (AllowPrivateAccess = "true"))
	float HitForceOnTrapdoorDoor = 500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Force", meta = (AllowPrivateAccess = "true"))
	float HitForceOnStaticObject = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Force", meta = (AllowPrivateAccess = "true"))
	float HitForceOnBumper = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Force", meta = (AllowPrivateAccess = "true"))
	float CarHitForce = 3000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	bool bIsHit = false;

	FTimerHandle HitTimerHandle;
	TPair<float, float> CalculateCarHitForceMultiplier(ABumperCar* bumperCar, ABumperCar* otherCar) const;

	UFUNCTION()
	float CalculateBumperHitForcePercentage(ABumperCar* bumperCar) const;
	
	UFUNCTION()
	FVector CalculateBasicHitNormal(AActor* actor, AActor* otherActor) const;

	UFUNCTION()
	FVector CalculateCarHitNormal(ABumperCar* bumperCar, ABumperCar* otherCar) const;

	UFUNCTION()
	FVector CalculateHitLocation(AActor* actor, AActor* otherActor) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BounceOnCarHit(const FHitResult& hit);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BounceOnBumperHit(const FHitResult& hit);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BounceOnStaticObject(const FHitResult& hit);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BounceOnTrapDoorObject(const FHitResult& hit);
};
