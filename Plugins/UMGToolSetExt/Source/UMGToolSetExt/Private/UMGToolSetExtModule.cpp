#include "UMGToolSetExtModule.h"
#include "Misc/CoreDelegates.h"
#include "ToolsetRegistry/UToolsetRegistry.h"
#include "UMGToolSetExt.h"

#define LOCTEXT_NAMESPACE "UMGToolSetExt"

void FUMGToolSetExtModule::StartupModule()
{
	FCoreDelegates::OnAllModuleLoadingPhasesComplete.AddRaw(
		this, &FUMGToolSetExtModule::OnAllModuleLoadingPhasesComplete);
	FCoreDelegates::OnPreExit.AddRaw(
		this, &FUMGToolSetExtModule::OnPreExit);
}

void FUMGToolSetExtModule::ShutdownModule()
{
	FCoreDelegates::OnAllModuleLoadingPhasesComplete.RemoveAll(this);
	FCoreDelegates::OnPreExit.RemoveAll(this);
}

void FUMGToolSetExtModule::OnAllModuleLoadingPhasesComplete()
{
	UToolsetRegistry::RegisterToolsetClass(UUMGToolSetExt::StaticClass());
}

void FUMGToolSetExtModule::OnPreExit()
{
	UToolsetRegistry::UnregisterToolsetClass(UUMGToolSetExt::StaticClass());
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUMGToolSetExtModule, UMGToolSetExt)
