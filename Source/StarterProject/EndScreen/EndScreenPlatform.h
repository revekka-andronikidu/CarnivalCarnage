// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndScreenPlatform.generated.h"

class UArrowComponent;
class UTextRenderComponent;

UCLASS()
class STARTERPROJECT_API AEndScreenPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEndScreenPlatform();
	void SpawnCharacter(APlayerController* PC);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Lobby", meta = (ExposeOnSpawn = true))
	UArrowComponent* PlayerSpawnArrow;

	UTextRenderComponent* TextComp;

	APlayerController* PlayerController;
	AActor* CurrentCharacter;

	//TEMP - will be replaced by selected player class (get from PC)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby")
	TSubclassOf<AActor> LobbyCharacterClass;




};
