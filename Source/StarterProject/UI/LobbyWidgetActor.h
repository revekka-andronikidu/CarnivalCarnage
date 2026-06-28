// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyWidgetActor.generated.h"

UCLASS()
class STARTERPROJECT_API ALobbyWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbyWidgetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
