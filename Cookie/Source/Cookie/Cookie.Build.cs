// Copyright (c) 2026 Jasper Drescher. All rights reserved.

using UnrealBuildTool;

public class Cookie : ModuleRules
{
	public Cookie(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"RHI",
			"AIModule",
			"MotionWarping"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"AdvancedSessions",
			"AdvancedSteamSessions",
			"CommonLoadingScreen"
		});

		PublicIncludePaths.AddRange(new string[] {
			"Cookie",
			"Cookie/Variant_Combat",
			"Cookie/Variant_Combat/AI",
			"Cookie/Variant_Combat/Animation",
			"Cookie/Variant_Combat/Gameplay",
			"Cookie/Variant_Combat/Interfaces",
			"Cookie/Variant_Combat/UI",
		});

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
	}
}
