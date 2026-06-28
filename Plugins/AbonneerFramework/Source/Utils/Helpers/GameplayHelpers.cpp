#include "GameplayHelpers.h"
#include <iostream>
#include <sstream>
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogGameplayHelpers);

void UGameplayHelpers::ServerTravelToLevel(UWorld* world, TSoftObjectPtr<UWorld> worldToLoad)
{
    // Get Level URL
    std::stringstream fullPath(std::string(TCHAR_TO_UTF8(*worldToLoad.ToSoftObjectPath().ToString())));
    std::string shortPath;
    getline(fullPath, shortPath, '.');

    world->ServerTravel(shortPath.c_str(), false, false);
}
void UGameplayHelpers::ServerTravelToLevel(UWorld* world, TSoftObjectPtr<UWorld> worldToLoad, TSubclassOf<AGameModeBase> gameModeToLoad)
{
    if (!world)
    {
        UE_LOG(LogGameplayHelpers, Warning, TEXT("Invalid world or level to load."));
        return;
    }

    // Get the short path of the level
    FString fullPath = worldToLoad.ToSoftObjectPath().ToString();
    FString shortPath;
    fullPath.Split(TEXT("."), &shortPath, nullptr);

    // Check if a valid game mode was provided
    FString travelURL = shortPath;
    if (gameModeToLoad)
    {
        // Add the game mode to the travel URL
        FString gameModePath = gameModeToLoad->GetPathName();
        travelURL = FString::Printf(TEXT("%s?game=%s"), *shortPath, *gameModePath);
    }

    // Perform server travel with the constructed URL
    world->ServerTravel(*travelURL, false, false);
}

bool UGameplayHelpers::IsWithEditor()
{
#if WITH_EDITOR
    return true;
#else
    return false;
#endif
}
