// Copyright (c) 2026 Jasper Drescher. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CookieEditorTarget : TargetRules
{
	public CookieEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		ExtraModuleNames.Add("Cookie");
	}
}
