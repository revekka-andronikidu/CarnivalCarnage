// Copyright Abonneer, Inc. All Rights Reserved.


#include "SettingsWidget.h"

void USettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_CloseSettings)
	{
		Button_CloseSettings->OnClicked.AddDynamic(this, &USettingsWidget::OnButton_CloseClicked);
	}

}

void USettingsWidget::OnButton_CloseClicked()
{
	RemoveFromParent();
}