// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndScreenWidget.generated.h"

class UButton;

UCLASS()
class STARTERPROJECT_API UEndScreenWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButton_ExitClicked();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> MainMenu;

	/*UFUNCTION()
	void OnButton_ReplayClicked();*/

public:
	/*UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_Replay;*/

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_Exit;

	
};
