// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class ABumperCar;

DECLARE_LOG_CATEGORY_EXTERN(LogHealthComponent, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, ABumperCar*, DeadActor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STARTERPROJECT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	UPROPERTY()
	FOnDeathSignature OnDeathEvent;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void SetupHealthComponent(int health);

	UFUNCTION()
	int GetCurrentHealth() const { return CurrentHealth; }
	
	UFUNCTION()
	void DoDamage(int damage);

	UFUNCTION()
	bool GetIsDead() const { return bIsDead; }

	UFUNCTION()
	bool Heal(float deltaTime);

	UFUNCTION(Server, Reliable)
	void Server_KillOwner();

	UFUNCTION()
	void ResetComponent();

	UFUNCTION() void SetCanHeal(bool canHeal) { bCanHeal = canHeal; }

	UFUNCTION()
	void OnRep_IsDead();

	UFUNCTION()
	void OnRep_CurrentHealth();

private:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	int CurrentHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	int MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float HealthRegenRate = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float AccumulatedRegenCounter = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_IsDead, EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	bool bIsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	bool bCanHeal = false;

	UFUNCTION()
	void HandleDeath();
};
