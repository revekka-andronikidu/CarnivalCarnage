// Copyright Abonneer, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StarterProject : ModuleRules
{
	public StarterProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemUtils"  });
        PublicDependencyModuleNames.AddRange(new string[] { "EnhancedInput", "UMG", "Niagara" });
        PublicDependencyModuleNames.AddRange(new string[] { "NavigationSystem" });
        PublicDependencyModuleNames.AddRange(new string[] { "PhysicsCore" });
        PublicDependencyModuleNames.AddRange(new string[] { "CommonUI", "Slate", "SlateCore", "MoviePlayer" });
        PublicDependencyModuleNames.AddRange(new string[] { "Framework", "Online", "Utils", "SmoothSyncPlugin", "Niagara" });
    }
}
