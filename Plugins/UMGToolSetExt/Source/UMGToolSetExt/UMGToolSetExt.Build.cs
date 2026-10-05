// UMG ToolSet Extensions

using UnrealBuildTool;

public class UMGToolSetExt : ModuleRules
{
	public UMGToolSetExt(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"ToolsetRegistry",
				"UMG",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"UnrealEd",
				"Slate",
				"SlateCore",
				"UMGEditor",
				"BlueprintGraph",
				"AssetRegistry",
			}
		);
	}
}
