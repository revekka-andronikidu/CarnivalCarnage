// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StarterProject/Lobby/LobbyCharacterActor.h"
#include "LobbyPlayerWindow.generated.h"


class UArrowComponent;
class AReadyDisplay;
class ALobbyCharacterActor;

UCLASS()
class STARTERPROJECT_API ALobbyPlayerWindow : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALobbyPlayerWindow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	APlayerController* PlayerController;
	UPROPERTY()
	ALobbyCharacterActor* CurrentCharacter;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby")
	TSubclassOf<ALobbyCharacterActor> LobbyCharacterClass;*/

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Lobby", meta = (ExposeOnSpawn = true))
	UArrowComponent* PlayerSpawnArrow;


	UPROPERTY()
	AReadyDisplay* LinkedSign;


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:
	bool HasPlayerAssigned() { return PlayerController != nullptr; };

	UFUNCTION()
	void SpawnCharacter(APlayerController* PC);
	APlayerController* GetAssignedPC() { return PlayerController; };

	UFUNCTION()
	void Clear();

	APlayerController* GetPC();


	void SetReady(bool isReady);

	//bool GetIsReady() { return IsReady; };

private:
	void LinkReadyDisplay();

	UPROPERTY(ReplicatedUsing = OnRep_IsReady)
	bool IsReady = false;

	UFUNCTION()
	void OnRep_IsReady();

};
