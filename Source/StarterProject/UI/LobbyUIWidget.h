// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUIWidget.generated.h"

/**
 * 
 */
class UButton;
UCLASS()
class STARTERPROJECT_API ULobbyUIWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;


	UFUNCTION(BlueprintCallable) void OnSequenceFinished();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStartClickedEvent();

	UFUNCTION()
	void OnButton_StartMatchClicked();

	UFUNCTION()
	void OnButton_ReadyClicked();

	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_StartMatch;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_ToggleReady;

	void ToggleStartEnabled(bool enabled);

	UFUNCTION(BlueprintImplementableEvent)
	void ReadyStatusChangedEvent();

	UPROPERTY(BlueprintReadOnly)
	bool IsEveryoneReady = false;
};
