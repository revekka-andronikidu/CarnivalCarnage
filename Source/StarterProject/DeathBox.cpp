// Copyright Abonneer, Inc. All Rights Reserved.


#include "DeathBox.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "BumperCar.h"
#include "BumperCarMovementComponent.h"
#include "StarterProject/GameModes/GameplayGameMode.h"

// Sets default values
ADeathBox::ADeathBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBoxPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBoxPtr;
	//CollisionBoxPtr->SetCollisionProfileName("Trigger");

	CollisionBoxPtr->InitBoxExtent(CollisionBoxInitSize);


	CollisionBoxPtr->OnComponentBeginOverlap.AddDynamic(this, &ADeathBox::OnBeginOverlap);

}

// Called when the game starts or when spawned
void ADeathBox::BeginPlay()
{
	Super::BeginPlay();
	
	//CollisionBoxPtr->OnComponentBeginOverlap.AddDynamic(this, &ADeathBox::OnBeginOverlap);
}

// Called every frame
void ADeathBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADeathBox::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
		return;

	if (!OtherActor || !OtherComp)
		return;

	ABumperCar* Car = Cast<ABumperCar>(OtherActor);
	if (!Car)
		return;

	if (OtherComp != Car->GetCarCollider())
	{
		UE_LOG(LogTemp, Warning, TEXT("Ignored overlap from component: %s"), *OtherComp->GetName());
		return;
	}

	// ------------ COOLDOWN ------------------
	float Now = GetWorld()->GetTimeSeconds();
	if (Now - Car->LastDeathBoxTime < 3.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ignored DeathBox overlap (cooldown). Car: %s"), *Car->GetName());
		return; 
	}

	Car->LastDeathBoxTime = Now; 
	// -------------------------


	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Yellow,
			FString::Printf(TEXT("Death box overlap with: %s"), *OtherActor->GetName())
		);
	}

	UE_LOG(LogTemp, Log, TEXT("DeathBox overlap -> Server kills car: %s | Component: %s"),*Car->GetName(), OtherComp ? *OtherComp->GetName() : TEXT("NO COMPONENT"));

	Car->Server_HandleDeathBox();
}
