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
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Cookie",
			"Cookie/Variant_Platforming",
			"Cookie/Variant_Platforming/Animation",
			"Cookie/Variant_Combat",
			"Cookie/Variant_Combat/AI",
			"Cookie/Variant_Combat/Animation",
			"Cookie/Variant_Combat/Gameplay",
			"Cookie/Variant_Combat/Interfaces",
			"Cookie/Variant_Combat/UI",
			"Cookie/Variant_SideScrolling",
			"Cookie/Variant_SideScrolling/AI",
			"Cookie/Variant_SideScrolling/Gameplay",
			"Cookie/Variant_SideScrolling/Interfaces",
			"Cookie/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
