// Copyright Abonneer. All Rights Reserved.

using UnrealBuildTool;

public class Framework : ModuleRules
{
	public Framework(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        //// Default Unreal Engine Stuff
        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput", "UMG", "Niagara", "CommonInput", "InputCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "ApplicationCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "NavigationSystem" });
        PrivateDependencyModuleNames.AddRange(new string[] { "PhysicsCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "CommonUI"});
        PrivateDependencyModuleNames.AddRange(new string[] { "DeveloperSettings" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "GameplayTags" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Voice", "VoiceChat" });

        PublicDependencyModuleNames.AddRange(new string[] { "MoviePlayer" });

        // Other Modules
        PrivateDependencyModuleNames.AddRange(new string[] { "Utils", "SmoothSyncPlugin" });

        // Online
        PrivateDependencyModuleNames.AddRange(new string[] { "Online" });
        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemNull", "OnlineSubsystemSteam", "OnlineSubsystemUtils", "Networking", "Sockets" });
        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "Sockets", "Networking", "OnlineSubsystemUtils" });

        //// Steam Online Stuff
        PublicDefinitions.Add("WITH_ADVANCED_SESSIONS=1");
        PublicDefinitions.Add("WITH_ADVANCED_STEAM_SESSIONS=1");

        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemUtils", "Networking", "Sockets", /*"Voice", "OnlineSubsystemSteam"*/ });
        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "Sockets", "Networking", "OnlineSubsystemUtils" /*"Voice", "Steamworks","OnlineSubsystemSteam"*/});

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Linux) || (Target.Platform == UnrealTargetPlatform.Mac))
        {
            PublicDependencyModuleNames.AddRange(new string[] { "Steamworks", "SteamShared", "OnlineSubsystemSteam" });
        }

		// Settings
		if (Target.bBuildEditor == true)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
			PrivateDependencyModuleNames.AddRange(new string[] {
				"Settings"
			});
		}
	}
}
