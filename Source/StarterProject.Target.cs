// Copyright Abonneer, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class StarterProjectTarget : TargetRules
{
	public StarterProjectTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("StarterProject");
	}
}
