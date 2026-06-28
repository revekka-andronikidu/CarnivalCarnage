using UnrealBuildTool;

public class Online : ModuleRules
{
    public Online(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDefinitions.Add("WITH_ADVANCED_SESSIONS=1");
        PublicDefinitions.Add("WITH_ADVANCED_STEAM_SESSIONS=1");

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "CoreUObject", "OnlineSubsystemUtils", "Networking", "Sockets", /*"Voice", "OnlineSubsystemSteam"*/ });
        PublicDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "Sockets", "Networking", "OnlineSubsystemUtils", "Voice" });

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Linux) || (Target.Platform == UnrealTargetPlatform.Mac))
        {
            PublicDependencyModuleNames.AddRange(new string[] { "Steamworks", "SteamShared", "OnlineSubsystemSteam" });
            PrivateDependencyModuleNames.AddRange(new string[] { "Steamworks", "SteamShared", "OnlineSubsystemSteam" });
        }




        PublicDefinitions.Add("ONLINESUBSYSTEMSTEAM_PACKAGE=1");
        PrivateDefinitions.Add("ONLINESUBSYSTEMSTEAM_PACKAGE=1");
        PrivateDefinitions.Add("STEAMSHARED_PACKAGE=1");

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "NetCore",
                "AudioMixer",
                "OnlineBase",
                "Json",
                "PacketHandler",
                "Projects",
                "SteamShared"
            }
        );

        AddEngineThirdPartyPrivateStaticDependencies(Target, "Steamworks");
    }
}