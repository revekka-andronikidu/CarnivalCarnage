// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ALobbyUIActor.generated.h"

UCLASS()
class STARTERPROJECT_API AALobbyUIActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AALobbyUIActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
