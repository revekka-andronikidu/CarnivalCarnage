#include "Online.h"

#include "OnlineSubsystemSteam.h"
#include "SteamSharedModule.h"
#include "Misc/CommandLine.h"

#include "Modules/ModuleManager.h"
#include "OnlineSubsystem.h"

#define LOCTEXT_NAMESPACE "FOnlineModule"

void FOnlineModule::StartupModule()
{
	
}

void FOnlineModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FOnlineModule, Online)