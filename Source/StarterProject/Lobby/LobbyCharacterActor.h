// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyCharacterActor.generated.h"

UCLASS()
class STARTERPROJECT_API ALobbyCharacterActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbyCharacterActor();
	float GetColor();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float PlayerColor = 0.0f;

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	USkeletalMeshComponent* MeshComp;


};
