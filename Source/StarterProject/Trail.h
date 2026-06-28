// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trail.generated.h"

class USplineComponent;
class UBoxComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UTrailComponent;
class UTrailCollisionComponent;
class UBumperCarMovementComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTrailActor, Log, All);

UCLASS()
class STARTERPROJECT_API ATrail : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrail();

	void ResetTrail();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(NetMulticast, Reliable) void Multicast_ActivateTrailFX(bool bActivate);

	UFUNCTION() void SpawnPoint();
	UFUNCTION() void DeletePoint();
	UFUNCTION(NetMulticast, Reliable) void Multicast_SetHueShift(float carColor);

	UFUNCTION() void OnHit();
	UFUNCTION() void DeleteCollisionOnHit(UPrimitiveComponent* overlappedComponent);

	UFUNCTION() bool GetOwnerIsHit() const { return bOwnerIsHit; }
	UFUNCTION() TArray<UBoxComponent*> GetTrailCollisions();

	UFUNCTION(NetMulticast, Reliable) void Multicast_SwitchTrailEffect(bool toSlime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> SplinePtr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UBoxComponent>> CollisionComponents;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraComponent> SpecialEffectsPtr;
	
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> LightningEffectAsset;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> FireTrailEffectAsset;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> SlimeEffectAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTrailComponent> TrailComponentPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTrailCollisionComponent> TrailCollisionLogicComponentPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	FTimerHandle HitTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	bool bOwnerIsHit = false;
	
	UPROPERTY()
	UBumperCarMovementComponent* OwnerMovementComponentPtr;

	UFUNCTION() void ResetTrailTimer();


	UPROPERTY(ReplicatedUsing = OnRep_OnColor)
	float PlayerColor = 0.f;

	UFUNCTION() void OnRep_OnColor();

	//UFUNCTION() void OnRep_HasBeenHit();
};
