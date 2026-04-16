// Copyright (c) 2026 Jasper Drescher. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CookieServerTarget : TargetRules
{
	public CookieServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;

		ProjectDefinitions.AddRange(
			[
				"UE_PROJECT_STEAMSHIPPINGID=480",
				"UE_PROJECT_STEAMPRODUCTNAME=480",
				"UE_PROJECT_STEAMGAMEDIR=Cookie",
				"UE_PROJECT_STEAMGAMEDESC=Cookie"
			]);

		ExtraModuleNames.Add("Cookie");
	}
}
