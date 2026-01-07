// Copyright Epic Games, Inc. All Rights Reserved.

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
			"RHI"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"AdvancedSessions",
			"AdvancedSteamSessions",
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
