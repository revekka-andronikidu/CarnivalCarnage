// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoostComponent.generated.h"

class UBumperCarMovementComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogBoost, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STARTERPROJECT_API UBoostComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBoostComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION() void ResetComponent();

	UFUNCTION() void Boost(bool isCompleted);
	UFUNCTION(NetMulticast, Reliable) void Multicast_EmptyBoost();
	UFUNCTION(NetMulticast, Reliable) void Multicast_AbsorbBoost(bool isActive);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost", meta = (AllowPrivateAccess = "true"))
	float CurrentBoostAmount = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost", meta = (AllowPrivateAccess = "true"))
	float MaxBoostAmount = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost", meta = (AllowPrivateAccess = "true"))
	float BoostConsumptionRate = 150.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost", meta = (AllowPrivateAccess = "true"))
	float BoostAbsorptionRate = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost", meta = (AllowPrivateAccess = "true"))
	UBumperCarMovementComponent* OwnerMovementComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost", meta = (AllowPrivateAccess = "true"))
	bool bIsAbsorbingBoost = false;

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_PoleChargeEffect();
};
