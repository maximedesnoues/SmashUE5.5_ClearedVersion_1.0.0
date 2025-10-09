// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SmashUE : ModuleRules
{
	public SmashUE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"DeveloperSettings"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
            "LocalMultiplayer"
        });
	}
}
