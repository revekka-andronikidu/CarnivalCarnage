#include "Utils.h"

#define LOCTEXT_NAMESPACE "FUtilsModule"

void FUtilsModule::StartupModule()
{
	this->LastFrameNumberWeTicked = INDEX_NONE;

}

void FUtilsModule::ShutdownModule()
{
}

void FUtilsModule::Tick(float DeltaTime)
{
	if (LastFrameNumberWeTicked != GFrameCounter)
	{
		LastFrameNumberWeTicked = GFrameCounter;
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUtilsModule, Utils)