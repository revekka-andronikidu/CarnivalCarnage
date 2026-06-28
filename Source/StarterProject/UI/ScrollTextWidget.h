// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScrollTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class STARTERPROJECT_API UScrollTextWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetScrollName(const FString& name);
};
