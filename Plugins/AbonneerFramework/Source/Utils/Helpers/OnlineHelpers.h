#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineHelpers.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOnlineHelpers, Log, All);

UCLASS()
class UTILS_API UOnlineHelpers : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void OpenInviteFriendsOverlay();

};
