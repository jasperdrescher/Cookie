// Copyright (c) 2026 Jasper Drescher. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CookieGameEOSTarget : TargetRules
{
	public CookieGameEOSTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;

		ExtraModuleNames.Add("Cookie");
		CustomConfig = "EOS";

		EnablePlugins.AddRange(
			new string[]
			{
				"OnlineServicesEOS",
				"OnlineSubsystemEOS"
			}
		);
	}
}
