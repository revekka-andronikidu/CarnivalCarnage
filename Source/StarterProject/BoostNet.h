// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoostNet.generated.h"

class UBoxComponent;
class ABumperCar;

DECLARE_LOG_CATEGORY_EXTERN(LogBoostNet, Log, All);

UCLASS()
class STARTERPROJECT_API ABoostNet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoostNet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "FX")
	void OnCarBeganOverlap();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetActive(bool isActive);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoostNetCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BoostNetMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FX", meta = (AllowPrivateAccess = "true"))
	int NumberOfLightningPoints = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FX", meta = (AllowPrivateAccess = "true"))
	TArray<FVector> LightningPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FX", meta = (AllowPrivateAccess = "true"))
	FVector Size = FVector{ 323.f, 500.f, 350.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	bool bIsActive = true;

	// Floating variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floating", meta = (AllowPrivateAccess = "true"))
	float FloatSpeed = 0.f;       

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floating", meta = (AllowPrivateAccess = "true"))
	float FloatAmplitude = 5.f;   
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floating", meta = (AllowPrivateAccess = "true"))
	float PitchSpeed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floating", meta = (AllowPrivateAccess = "true"))
	float RollSpeed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floating", meta = (AllowPrivateAccess = "true"))
	float RotationAmplitude = 3.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floating", meta = (AllowPrivateAccess = "true"))
	FVector StartingLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floating", meta = (AllowPrivateAccess = "true"))
	FRotator StartingRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floating", meta = (AllowPrivateAccess = "true"))
	float GameTime = 0.f;

	UFUNCTION()
	void RandomizeLightningPoints();

	UFUNCTION()
	void FloatBoostNet(float deltaTime);
};
