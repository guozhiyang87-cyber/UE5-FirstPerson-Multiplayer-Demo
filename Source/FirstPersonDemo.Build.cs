using UnrealBuildTool;

public class FirstPersonDemo : ModuleRules
{
    public FirstPersonDemo(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(new string[]
        {
            "FirstPersonDemo/Public/Character",
            "FirstPersonDemo/Public/Game",
            "FirstPersonDemo/Public/Weapon",
            "FirstPersonDemo/Public/UI",
            "FirstPersonDemo/Public/Utilities"
        });

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "GameplayTasks",
            "AIModule",
            "NavigationSystem",
            "UMG"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
