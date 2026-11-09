using UnrealBuildTool;

public class NewEnemyAnimationTools : ModuleRules
{
    public NewEnemyAnimationTools(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine" });
        PrivateDependencyModuleNames.AddRange(new[] { "UnrealEd", "BlueprintGraph", "AnimGraph", "KismetCompiler",
            "AIModule", "AIGraph", "BehaviorTreeEditor", "MaterialEditor", "UMG", "UMGEditor", "Slate", "SlateCore", "RenderCore", "RHI",
            "TeamPotato", "GameplayAbilities", "GameplayTags" });
    }
}
