#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * UMGToolSetExt module - project-local AI assistant toolset that extends
 * UMGToolSet with higher-level UMG widget operations.
 */
class FUMGToolSetExtModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Called when all modules have finished loading */
	void OnAllModuleLoadingPhasesComplete();

	/** Called before application exit */
	void OnPreExit();
};
