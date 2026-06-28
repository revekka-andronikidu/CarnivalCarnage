// Copyright Abonneer, Inc. All Rights Reserved.


#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameModes/GameplayGameMode.h"
#include "BumperCar.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogHealthComponent);
UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentHealth < MaxHealth)
	{
		if (ABumperCar* Car = Cast<ABumperCar>(GetOwner()))
		{
			if (!Car->HasAuthority()) { return; }
			//UE_LOG(LogHealthComponent, Log, TEXT("state is %s"), *StaticEnum<EBumperCarStateType>()->GetNameStringByValue((int64)Car->BumperCarComponentPtr->GetStateType()))
			if (bCanHeal)
			{
				if (Heal(DeltaTime)) 
				{
					float healthPercentage = static_cast<float>(CurrentHealth) / static_cast<float>(MaxHealth);
					Car->UpdateDamageMaterialEffect(); 
				}
			}
		}
	}
}

void UHealthComponent::SetupHealthComponent(int health)
{
	MaxHealth = health;
	CurrentHealth = MaxHealth;
}

void UHealthComponent::DoDamage(int damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - damage, 0, MaxHealth);
	GEngine->AddOnScreenDebugMessage(4, 2.f, FColor::Black, FString::FromInt(CurrentHealth));
	if (ABumperCar* Car = Cast<ABumperCar>(GetOwner()))
	{
		Car->UpdateDamageMaterialEffect(); // <- manual call on server/ clients through OnRep

		UE_LOG(LogHealthComponent, Log, TEXT("%s current health is: %d."), *GetOwner()->GetName(), CurrentHealth);
		
		if (CurrentHealth <= 0 && !bIsDead)
		{
			//Car->OnFireDeathEvent();
			AGameplayGameMode* GM = Cast<AGameplayGameMode>(GetWorld()->GetAuthGameMode());
			if (GM)
			{
				UE_LOG(LogHealthComponent, Log, TEXT("%s died. Called server kill player function."), *GetOwner()->GetName());
				bIsDead = true;
				GM->KillPlayer(Cast<ABumperCar>(GetOwner()));
			}
		}
	}
}

bool UHealthComponent::Heal(float deltaTime)
{
	if (CurrentHealth >= MaxHealth) { return false; }
	
	AccumulatedRegenCounter += HealthRegenRate * deltaTime;
	if (AccumulatedRegenCounter >= 1)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth + 1, 0, MaxHealth);
		AccumulatedRegenCounter -= 1.f;
		UE_LOG(LogHealthComponent, Log, TEXT("%s healed to %d health."), *GetOwner()->GetName(), CurrentHealth);
	}
	return true;
}

void UHealthComponent::Server_KillOwner_Implementation()
{
	if (bIsDead) 
	{ 
		UE_LOG(LogHealthComponent, Warning, TEXT("%s is already dead. (KillOwner)"), *GetOwner()->GetName());
		return; 
	}

	CurrentHealth = 0;
	bIsDead = true;

	HandleDeath();
}

void UHealthComponent::ResetComponent()
{
	UE_LOG(LogHealthComponent, Log, TEXT("Health reset for: %s"), *GetOwner()->GetName());

	bIsDead = false;
	CurrentHealth = MaxHealth;
	if (ABumperCar* Car = Cast<ABumperCar>(GetOwner()))
	{
		Car->UpdateDamageMaterialEffect(); // <- manual call on server/ clients through OnRep
	}
}

void UHealthComponent::HandleDeath()
{
	if (ABumperCar* car = Cast<ABumperCar>(GetOwner()))
	{
		UE_LOG(LogHealthComponent, Log, TEXT("On death event broadcasted for: %s"), *GetOwner()->GetName());
		OnDeathEvent.Broadcast(car);
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, bIsDead);
	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}


void UHealthComponent::OnRep_IsDead()
{
	if (bIsDead)
	{
		// Only client-side reactions: VFX, UI, sounds, movement freeze
		UE_LOG(LogHealthComponent, Warning, TEXT("Client sees %s as dead"), *GetOwner()->GetName());
	}
	else
	{
		UE_LOG(LogHealthComponent, Warning, TEXT("Client sees %s revived/reset"), *GetOwner()->GetName());
	}
}



void UHealthComponent::OnRep_CurrentHealth()
{
	if (ABumperCar* car = Cast<ABumperCar>(GetOwner()))
	{
		UE_LOG(LogHealthComponent, Log, TEXT("%s called OnRep_CurrentHealth."), *GetOwner()->GetName());
		car->UpdateDamageMaterialEffect();
	}
}