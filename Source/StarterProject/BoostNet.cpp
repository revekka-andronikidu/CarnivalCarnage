// Copyright Abonneer, Inc. All Rights Reserved.


#include "BoostNet.h"
#include "Components/BoxComponent.h"
#include "BumperCar.h"
#include "BoostComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogBoostNet);

// Sets default values
ABoostNet::ABoostNet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoostNetCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoostNetCollision"));
	BoostNetCollision->InitBoxExtent(Size);
	RootComponent = BoostNetCollision;

	BoostNetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoostNetMesh"));
	BoostNetMesh->SetupAttachment(RootComponent);
	BoostNetMesh->SetRelativeLocation(FVector(0.f, 0.f, Size.Z));

	bReplicates = true;
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void ABoostNet::BeginPlay()
{
	Super::BeginPlay();

	BoostNetCollision->SetGenerateOverlapEvents(true);
	BoostNetCollision->OnComponentBeginOverlap.AddDynamic(this, &ABoostNet::OnBeginOverlap);
	BoostNetCollision->OnComponentEndOverlap.AddDynamic(this, &ABoostNet::OnEndOverlap);

	StartingLocation = GetActorLocation();
	StartingRotation = GetActorRotation();

	FloatSpeed = FMath::FRandRange(1.5f, 2.f);
	PitchSpeed = FMath::FRandRange(1.f, 1.5f);
	RollSpeed = FMath::FRandRange(1.f, 1.5f);
}

// Called every frame
void ABoostNet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FloatBoostNet(DeltaTime);
	//DrawDebugBox(
	//	GetWorld(),
	//	BoostNetCollision->GetComponentLocation(),
	//	BoostNetCollision->GetScaledBoxExtent(),
	//	BoostNetCollision->GetComponentQuat(),
	//	FColor::Cyan,
	//	false,
	//	-1.f,  // show only for 1 frame
	//	0,
	//	1.0f
	//);

}

void ABoostNet::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABumperCar* BumperCar = Cast<ABumperCar>(OtherActor);
	
	if (!BumperCar || !bIsActive) { return; }

	if (!BumperCar->HasAuthority()) { return; }

	if (BumperCar and OtherComp->ComponentHasTag("CarCollision"))
	{
		auto* boostComponent = Cast<UBoostComponent>(BumperCar->GetComponentByClass(UBoostComponent::StaticClass()));
		if (boostComponent)
		{
			boostComponent->Multicast_AbsorbBoost(true);
		}
		RandomizeLightningPoints();
		BumperCar->ActivateBoostNetEffect(LightningPoints, true);
	}
}

void ABoostNet::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABumperCar* BumperCar = Cast<ABumperCar>(OtherActor);
	if (!BumperCar) { return; }

	if (!BumperCar->HasAuthority()) { return; }

	if (BumperCar and OtherComp->ComponentHasTag("CarCollision"))
	{
		auto* boostComponent = Cast<UBoostComponent>(BumperCar->GetComponentByClass(UBoostComponent::StaticClass()));
		if (boostComponent)
		{
			boostComponent->Multicast_AbsorbBoost(false);
		}
		BumperCar->ActivateBoostNetEffect(LightningPoints, false);
	}
}

void ABoostNet::Multicast_SetActive_Implementation(bool isActive)
{
	bIsActive = isActive;
	if (GetWorld())
	{
		TArray<AActor*> bumperCars;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABumperCar::StaticClass(), bumperCars);

		for (AActor* actor : bumperCars)
		{
			ABumperCar* bumperCar = Cast<ABumperCar>(actor);
			if (bumperCar)
			{
				UBoostComponent* boost = Cast<UBoostComponent>(bumperCar->GetComponentByClass(UBoostComponent::StaticClass()));
				boost->Multicast_AbsorbBoost(false);
			}
		}
	}
}

void ABoostNet::RandomizeLightningPoints()
{
	if (LightningPoints.Num() < NumberOfLightningPoints)
	{
		LightningPoints.SetNum(NumberOfLightningPoints);
	}

	for (FVector& location : LightningPoints)
	{
		location.X = FMath::FRandRange(-Size.X, Size.X);
		location.Y = FMath::FRandRange(-Size.Y, Size.Y);
		location.Z = Size.Z;

		UE_LOG(LogBoostNet, Log, TEXT("local location: %s"), *location.ToString());

		location = GetActorTransform().TransformPosition(location);

		UE_LOG(LogBoostNet, Log, TEXT("world location: %s"), *location.ToString());
	}

	for (const FVector& location : LightningPoints)
	{
		UE_LOG(LogBoostNet, Log, TEXT("final lightning point: %s"), *location.ToString());
	}
}

void ABoostNet::FloatBoostNet(float deltaTime)
{
	GameTime += deltaTime;

	float verticalOffset = FMath::Sin(GameTime * FloatSpeed) * FloatAmplitude;
	float pitchOffset = FMath::Sin(GameTime * PitchSpeed) * RotationAmplitude;
	float rollOffset = FMath::Sin(GameTime * RollSpeed) * RotationAmplitude;

	FVector newLocation = StartingLocation;
	newLocation.Z += verticalOffset;

	FRotator newRotation = StartingRotation;
	newRotation.Pitch += pitchOffset;
	newRotation.Roll += rollOffset;

	SetActorLocationAndRotation(newLocation, newRotation);
}

