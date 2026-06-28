#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FrameworkSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig)
class FRAMEWORK_API UFrameworkSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	UFrameworkSettings()
	{
	};

public:
	virtual FName GetContainerName() const override { return TEXT("Project"); };
	virtual FName GetCategoryName() const override { return TEXT("Abonneer"); };
	virtual FName GetSectionName() const override { return TEXT("Core"); };

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	bool bIsDemoBuild{ false };
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	bool bIsItchBuild{ false };
};
