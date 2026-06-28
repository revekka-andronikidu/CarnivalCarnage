#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayHelpers.generated.h"

class ACustomHUDBase;

DECLARE_LOG_CATEGORY_EXTERN(LogGameplayHelpers, Log, All);

UCLASS()
class UTILS_API UGameplayHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static void ServerTravelToLevel(UWorld* world, TSoftObjectPtr<UWorld> worldToLoad);
	static void ServerTravelToLevel(UWorld* world, TSoftObjectPtr<UWorld> worldToLoad, TSubclassOf<AGameModeBase> gameModeToLoad);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsWithEditor();
};
