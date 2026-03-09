// Copyright (c) 2026 Jasper Drescher. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CookieGameEOSTarget : CookieGameTarget
{
	public CookieGameEOSTarget(TargetInfo Target) : base(Target)
	{
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
