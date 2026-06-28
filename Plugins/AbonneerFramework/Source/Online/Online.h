#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FOnlineModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

	virtual bool SupportsDynamicReloading() override
	{
		return false;
	}

	virtual bool SupportsAutomaticShutdown() override
	{
		return false;
	}
protected:
private:

	/** Class responsible for creating instance(s) of the subsystem */
	class FOnlineFactorySteam* SteamFactory;
};

