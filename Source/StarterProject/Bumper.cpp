// Copyright Abonneer, Inc. All Rights Reserved.


#include "Bumper.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"

// Sets default values
ABumper::ABumper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBoxPtr = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionBox"));
	CollisionBoxPtr->InitSphereRadius(192.f);
	//CollisionBoxPtr->InitBoxExtent(FVector(200.f, 200.f, 60.f));
	RootComponent = CollisionBoxPtr;

	MeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshPtr->SetupAttachment(RootComponent);

	AudioComponentPtr = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponentPtr->SetupAttachment(RootComponent);
	AudioComponentPtr->bAutoActivate = false;

	bReplicates = true;
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void ABumper::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABumper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABumper::NetMulticast_PlaySoundCue_Implementation()
{
	if (AudioComponentPtr->GetSound())
	{
		AudioComponentPtr->Play();
	}
}

