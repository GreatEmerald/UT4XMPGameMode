namespace UnrealBuildTool.Rules
{
    public class XMPGameMode : ModuleRules
    {
	public XMPGameMode(TargetInfo Target)
	{
	    PrivateIncludePaths.Add("XMPGameMode/Private");
	    PublicDependencyModuleNames.AddRange(
		new string[]
		{
		    "Core",
		    "CoreUObject",
		    "Engine",
		    "UnrealTournament",
		    "InputCore",
		    "Slate",
		    "SlateCore",
		    "UMG"
		}
	    );
	}
    }
}
