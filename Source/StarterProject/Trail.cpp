// Copyright Abonneer, Inc. All Rights Reserved.


#include "Trail.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"
#include "TrailComponent.h"
#include "TrailCollisionComponent.h"
#include "BumperCarMovementComponent.h"
#include "BumperCar.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// niagara
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTrailActor);

// Sets default values
ATrail::ATrail()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplinePtr = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	RootComponent = SplinePtr;

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> lightningEffectAsset(TEXT("/game/ArtAssets/Effects/N_Lightning_Trail_System.N_Lightning_Trail_System"));
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> fireTrailEffectAsset(TEXT("/Game/ArtAssets/Effects/N_Fire_Trail_System.N_Fire_Trail_System"));
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> slimeEffectAsset(TEXT("/game/ArtAssets/Effects/N_SlimeTrail_System.N_SlimeTrail_System"));
	
	//LightningEffectAsset = lightningEffectAsset.Object;
	FireTrailEffectAsset = fireTrailEffectAsset.Object;
	SlimeEffectAsset = slimeEffectAsset.Object;
	
	SpecialEffectsPtr = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffect"));
	SpecialEffectsPtr->SetupAttachment(RootComponent);
	SpecialEffectsPtr->SetAsset(FireTrailEffectAsset);
	SpecialEffectsPtr->SetBoundsScale(2500.f);

	TrailComponentPtr = CreateDefaultSubobject<UTrailComponent>(TEXT("TrailComponent"));
	TrailCollisionLogicComponentPtr = CreateDefaultSubobject<UTrailCollisionComponent>(TEXT("CollisionLogicComponent"));

	bReplicates = true;
	bAlwaysRelevant = true;
}

void ATrail::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATrail, PlayerColor);
}

void ATrail::ResetTrail()
{
	for (auto Box : CollisionComponents)
	{
		if (Box)
		{
			Box->DestroyComponent();
		}
	}

	Multicast_SwitchTrailEffect(false);

	CollisionComponents.Empty();
	TrailComponentPtr->ResetComponent();
}

// Called when the game starts or when spawned
void ATrail::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATrail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!OwnerMovementComponentPtr)
	{
		if (auto ownerCar = Cast<ABumperCar>(GetOwner()))
		{
			if (!ownerCar->HasAuthority()) { return; }
			OwnerMovementComponentPtr = ownerCar->GetBumperCarMovementComponent();
		}
	}

	//DEBUG TO VISUALIZE THE TRAIL
	 //for (auto Box : CollisionComponents)
	 //{
	 //	if (!Box) continue;
	 //
	 //	DrawDebugBox(
	 //		GetWorld(),
	 //		Box->GetComponentLocation(),
	 //		Box->GetScaledBoxExtent(),
	 //		Box->GetComponentQuat(),
	 //		FColor::Cyan,
	 //		false,
	 //		-1.f,  // show only for 1 frame
	 //		0,
	 //		1.0f
	 //	);
	 //}

	if (OwnerMovementComponentPtr)
	{
		if (OwnerMovementComponentPtr->GetCurrentSpeed() <= 0.f)
		{
			Multicast_ActivateTrailFX(false);
		}
		else
		{
			Multicast_ActivateTrailFX(true);
		}
	}
}

void ATrail::Multicast_ActivateTrailFX_Implementation(bool bActivate)
{
	if (bActivate)
	{
		if (SpecialEffectsPtr->IsActive()) { return; }
		SpecialEffectsPtr->Activate();
	}
	else
	{
		if (!SpecialEffectsPtr->IsActive()) { return; }
		SpecialEffectsPtr->Deactivate();
	}
}

void ATrail::SpawnPoint()
{
	if (!HasAuthority()) { return; }

	SplinePtr->AddSplinePoint(GetActorLocation(), ESplineCoordinateSpace::World);
	UBoxComponent* boxCollision = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass());
	boxCollision->RegisterComponent();
	boxCollision->InitBoxExtent(FVector{50.f, 50.f, 50.f});
	boxCollision->SetWorldLocation(GetOwner()->GetActorLocation() + -225.f * GetActorForwardVector());
	boxCollision->SetWorldRotation(GetOwner()->GetActorRotation());
	boxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	boxCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore); // GhostCar
	boxCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap); // CarPlayer
	boxCollision->OnComponentBeginOverlap.AddDynamic(TrailCollisionLogicComponentPtr.Get(), &UTrailCollisionComponent::OnBeginOverlap);
	CollisionComponents.Add(boxCollision);
}

void ATrail::DeletePoint()
{
	if (!HasAuthority()) { return; }
	
	SplinePtr->RemoveSplinePoint(0);

	//TEMP COMMENTED OUT FOR DEBUG
	if (CollisionComponents[0])
	{
		CollisionComponents[0]->DestroyComponent();
	}

	CollisionComponents.RemoveAt(0);
}

void ATrail::OnHit()
{
	bOwnerIsHit = true;
	UE_LOG(LogTrailActor, Log, TEXT("OnHit function has been hit"));

	GetWorldTimerManager().SetTimer(
		HitTimerHandle,
		this,
		&ATrail::ResetTrailTimer,
		1.f,
		false
	);
}

void ATrail::DeleteCollisionOnHit(UPrimitiveComponent* overlappedComponent)
{
	int index = CollisionComponents.IndexOfByKey(overlappedComponent);
	if (index == INDEX_NONE) 
	{ 
		UE_LOG(LogTrailActor, Log, TEXT("Trail has %d collisionBoxes"), CollisionComponents.Num());
		UE_LOG(LogTrailActor, Warning, TEXT("Collision component not found in trail actor"));
		return; 
	}
	CollisionComponents[index]->DestroyComponent();
	CollisionComponents.RemoveAt(index);
	TrailComponentPtr->DeleteTimer(index);

	UE_LOG(LogTrailActor, Log, TEXT("Collision component at index %d removed from trail actor"), index);
}

TArray<UBoxComponent*> ATrail::GetTrailCollisions()
{
	TArray<UBoxComponent*> collisionBuffer;
	collisionBuffer.Reserve(CollisionComponents.Num());

	for (const TObjectPtr<UBoxComponent>& Collision : CollisionComponents)
	{
		collisionBuffer.Add(Collision.Get());
	}

	return collisionBuffer;
}

void ATrail::Multicast_SwitchTrailEffect_Implementation(bool toSlime)
{
	if (toSlime)
	{
		UE_LOG(LogTrailActor, Log, TEXT("Trail set to slime attempt"));
		if (SpecialEffectsPtr->GetAsset() == SlimeEffectAsset) { return; }
		SpecialEffectsPtr->SetAsset(SlimeEffectAsset);
		SpecialEffectsPtr->SetVariableFloat(FName("PlayerColor"), PlayerColor);
		UE_LOG(LogTrailActor, Log, TEXT("Trail set to slime, color: %f"), PlayerColor);
	}
	else
	{
		UE_LOG(LogTrailActor, Log, TEXT("Trail set to lightning attempt"));
		if (SpecialEffectsPtr->GetAsset() == FireTrailEffectAsset) { return; }
		SpecialEffectsPtr->SetAsset(FireTrailEffectAsset);
		SpecialEffectsPtr->SetVariableFloat(FName("PlayerColor"), PlayerColor);
		UE_LOG(LogTrailActor, Log, TEXT("Trail set to lightning"));
	}
}

void ATrail::ResetTrailTimer()
{
	bOwnerIsHit = false;
	GetWorldTimerManager().ClearTimer(HitTimerHandle);
}

void ATrail::Multicast_SetHueShift_Implementation(float carColor)
{
	PlayerColor = carColor;
	SpecialEffectsPtr->SetVariableFloat(FName("PlayerColor"), PlayerColor);

	//UE_LOG(LogTemp, Warning, TEXT("Player color changed to: %f for player: %s"), PlayerColor, *GetOwner()->GetName());
}
void ATrail::OnRep_OnColor()
{
	SpecialEffectsPtr->SetVariableFloat(FName("PlayerColor"), PlayerColor);
}

