// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "SettingsWidget.generated.h"


/**
 * 
 */
UCLASS()
class STARTERPROJECT_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_CloseSettings;

	UFUNCTION()
	void OnButton_CloseClicked();
	
};
