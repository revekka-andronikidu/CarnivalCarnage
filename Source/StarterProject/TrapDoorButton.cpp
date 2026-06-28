// Copyright Abonneer, Inc. All Rights Reserved.


#include "TrapDoorButton.h"
#include "Components/BoxComponent.h"
#include "BumperCar.h"
#include "TrapDoor.h"
#include "Components/CapsuleComponent.h"

DEFINE_LOG_CATEGORY(LogTrapDoorButton);

// Sets default values
ATrapDoorButton::ATrapDoorButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBoxPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBoxPtr->SetBoxExtent(FVector(128.f, 128.f, 16.f));
	RootComponent = CollisionBoxPtr;

	ButtonMeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMeshPtr->SetupAttachment(RootComponent);
	ButtonMeshPtr->SetRelativeLocation(FVector(0.f, 0.f, -20.f));

}

// Called when the game starts or when spawned
void ATrapDoorButton::BeginPlay()
{
	Super::BeginPlay();

	// physics/collision settings
	CollisionBoxPtr->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBoxPtr->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap); // GhostCar
	CollisionBoxPtr->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap); // CarPlayer
	ButtonMeshPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	for (TObjectIterator<ATrapDoor> Iterator; Iterator; ++Iterator)
	{
		ATrapDoor* TrapDoorPtr = *Iterator;
		if (TrapDoorPtr)
		{
			OnButtonPressed.AddDynamic(TrapDoorPtr, &ATrapDoor::OpenTrapDoor);
			UE_LOG(LogTrapDoorButton, Log, TEXT("Bound Trap Door %s to Trap Door Button %s"), *TrapDoorPtr->GetName(), *GetName());
		}
	}
	CollisionBoxPtr->OnComponentBeginOverlap.AddDynamic(this, &ATrapDoorButton::OnOverlapBegin);
	CollisionBoxPtr->OnComponentEndOverlap.AddDynamic(this, &ATrapDoorButton::OnOverlapEnd);
	
}

void ATrapDoorButton::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTrapDoorButton, Log, TEXT("Trap Door Button OnOverlap."));

	ABumperCar* Car = Cast<ABumperCar>(OtherActor);
	if (!Car)
		return;

	if(OtherComp != Car->GetCollisionBox())
	{
		UE_LOG(LogTemp, Warning, TEXT("Ignored overlap from component: %s"), *OtherComp->GetName());
		return;
	}	

	if (bIsPressed)
		return;

	bIsPressed = true;

	OnDoorButtonPressed();

	UE_LOG(LogTrapDoorButton, Log, TEXT("Trap Door Button Pressed by %s"), *OtherActor->GetName());
	OnButtonPressed.Broadcast();	
}

void ATrapDoorButton::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTrapDoorButton, Log, TEXT("Trap Door Button OnOverlap."));

	ABumperCar* Car = Cast<ABumperCar>(OtherActor);
	if (!Car)
		return;

	if (OtherComp != Car->GetCollisionBox())
	{
		UE_LOG(LogTemp, Warning, TEXT("Ignored overlap from component: %s"), *OtherComp->GetName());
		return;
	}
	UE_LOG(LogTrapDoorButton, Log, TEXT("Trap Door Button Pressed by %s"), *OtherActor->GetName());
	

	if (!bIsPressed)
		return;

	bIsPressed = false;

	//on released
	OnDoorButtonReleased();
}




// Called every frame
void ATrapDoorButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

