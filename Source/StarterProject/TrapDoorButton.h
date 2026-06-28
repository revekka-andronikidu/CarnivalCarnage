// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrapDoorButton.generated.h"

class UBoxComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTrapDoorButton, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggerSignature);

UCLASS()
class STARTERPROJECT_API ATrapDoorButton : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrapDoorButton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp,	AActor* OtherActor,	UPrimitiveComponent* OtherComp,	int32 OtherBodyIndex);

	UPROPERTY()
	bool bIsPressed = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintImplementableEvent)
	void OnDoorButtonPressed();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDoorButtonReleased();

private:
	UPROPERTY()
	FOnTriggerSignature OnButtonPressed;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ButtonMeshPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> CollisionBoxPtr;
};
