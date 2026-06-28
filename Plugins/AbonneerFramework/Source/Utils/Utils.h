#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Tickable.h"

class FUtilsModule : public IModuleInterface, public FTickableGameObject 
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    // FTickableGameObject interface
    virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	virtual bool IsTickableWhenPaused() const { return true; }
	virtual bool IsTickableInEditor() const { return false; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UUtils, STATGROUP_Tickables); }
protected:
	uint32 LastFrameNumberWeTicked;
};

