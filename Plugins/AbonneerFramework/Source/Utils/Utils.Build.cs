using UnrealBuildTool;

public class Utils : ModuleRules
{
    public Utils(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        // Default Unreal Engine Stuff
        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput", "UMG", "Niagara" });
        PrivateDependencyModuleNames.AddRange(new string[] { "NavigationSystem" });
        PrivateDependencyModuleNames.AddRange(new string[] { "PhysicsCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "CommonUI" });
        PrivateDependencyModuleNames.AddRange(new string[] { "DeveloperSettings" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "GameplayTags" });

        // Other Modules
        PrivateDependencyModuleNames.AddRange(new string[] { "SmoothSyncPlugin" });

        // Online
        PrivateDependencyModuleNames.AddRange(new string[] { "Online" });
        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemNull", "OnlineSubsystemSteam", "OnlineSubsystemUtils", "Networking", "Sockets" });
        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "Sockets", "Networking", "OnlineSubsystemUtils" });

        // Editor Specific Dependencies
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }
    }
}