// Copyright Abonneer, Inc. All Rights Reserved.


#include "TrapDoor.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"

// niagara
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

DEFINE_LOG_CATEGORY(LogTrapDoor);

// Sets default values
ATrapDoor::ATrapDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TrapDoorCollisionPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("TrapDoorCollision"));
	TrapDoorCollisionPtr->SetBoxExtent(FVector{ 350.f, 350.f, 40.f });
	RootComponent = TrapDoorCollisionPtr;

	TrapDoorBorderMeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapDoorBorderMesh"));
	TrapDoorBorderMeshPtr->SetupAttachment(RootComponent);

	LeftDoorMeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorMesh"));
	LeftDoorMeshPtr->SetupAttachment(TrapDoorBorderMeshPtr);
	LeftDoorMeshPtr->SetRelativeLocation(FVector(0.f, -290.f, -13.13f));
	LeftDoorMeshPtr->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

	LeftDoorCollisionPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftDoorCollision"));
	LeftDoorCollisionPtr->SetupAttachment(LeftDoorMeshPtr);
	LeftDoorCollisionPtr->InitBoxExtent(FVector{ 300.f, 150.f, 20.f });
	LeftDoorCollisionPtr->SetRelativeLocation(FVector(0.f, -120.f, -20.f));

	RightDoorMeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoorMesh"));
	RightDoorMeshPtr->SetupAttachment(TrapDoorBorderMeshPtr);
	RightDoorMeshPtr->SetRelativeLocation(FVector(0.f, 300.f, -13.13f));
	RightDoorMeshPtr->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

	RightDoorCollisionPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("RightDoorCollision"));
	RightDoorCollisionPtr->SetupAttachment(RightDoorMeshPtr);
	RightDoorCollisionPtr->InitBoxExtent(FVector{ 300.f, 150.f, 20.f });
	RightDoorCollisionPtr->SetRelativeLocation(FVector(0.f, 120.f, -20.f));

	AudioComponentOpenPtr = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioOpenComponent"));
	AudioComponentOpenPtr->SetupAttachment(RootComponent);
	AudioComponentOpenPtr->bAutoActivate = false;

	AudioComponentClosePtr = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioCloseComponent"));
	AudioComponentClosePtr->SetupAttachment(RootComponent);
	AudioComponentClosePtr->bAutoActivate = false;

	NiagaraOpenFXPtr = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraOpenFX"));
	NiagaraOpenFXPtr->SetupAttachment(RootComponent);
	NiagaraOpenFXPtr->bAutoActivate = false;

	NiagaraCloseFXPtr = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraCloseFX"));
	NiagaraCloseFXPtr->SetupAttachment(RootComponent);
	NiagaraCloseFXPtr->bAutoActivate = false;
}

// Called when the game starts or when spawned
void ATrapDoor::BeginPlay()
{
	Super::BeginPlay();

	TrapDoorCollisionPtr->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	TrapDoorCollisionPtr->SetGenerateOverlapEvents(false);
	TrapDoorBorderMeshPtr->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
	TrapDoorBorderMeshPtr->SetGenerateOverlapEvents(false);
	LeftDoorMeshPtr->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
	LeftDoorMeshPtr->SetGenerateOverlapEvents(false);
	RightDoorMeshPtr->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
	RightDoorMeshPtr->SetGenerateOverlapEvents(false);
	
	LeftDoorCollisionPtr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	LeftDoorCollisionPtr->SetGenerateOverlapEvents(true);
	RightDoorCollisionPtr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	RightDoorCollisionPtr->SetGenerateOverlapEvents(true);
}

// Called every frame
void ATrapDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATrapDoor::OpenTrapDoor()
{
	if (bIsOpen) { return; }

	bIsOpen = true;

	// actively open the door
	LeftDoorMeshPtr->SetRelativeRotation(FRotator(0.f, 180.f, -90.f));
	RightDoorMeshPtr->SetRelativeRotation(FRotator(0.f, 180.f, 90.f));
	TrapDoorCollisionPtr->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);

	GetWorldTimerManager().SetTimer(
		OpenTrapDoorTimeHandle,
		this,
		&ATrapDoor::CloseTrapDoor,
		OpenTime,
		false);

    if (AudioComponentOpenPtr) { AudioComponentOpenPtr->Play(); }
	if (NiagaraOpenFXPtr) { NiagaraOpenFXPtr->Activate(true); }

	UE_LOG(LogTrapDoor, Log, TEXT("%s: Trap Door Opened"), *GetName());
}

void ATrapDoor::CloseTrapDoor()
{
	bIsOpen = false;
	LeftDoorMeshPtr->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	RightDoorMeshPtr->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	TrapDoorCollisionPtr->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

	GetWorldTimerManager().ClearTimer(OpenTrapDoorTimeHandle);
	if (AudioComponentClosePtr) { AudioComponentClosePtr->Play(); }
	if (NiagaraCloseFXPtr) { NiagaraCloseFXPtr->Activate(true); }

	UE_LOG(LogTrapDoor, Log, TEXT("%s: Trap Door Closed"), *GetName());
}

