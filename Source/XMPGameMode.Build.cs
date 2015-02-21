namespace UnrealBuildTool.Rules
{
	public class XMPGameMode : ModuleRules
	{
		public XMPGameMode(TargetInfo Target)
		{
			PrivateIncludePaths.Add("Classes");
			PrivateIncludePaths.Add("Restricted");
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"UnrealTournament",
					"InputCore",
					"SlateCore",
				}
			);
		}
	}
}