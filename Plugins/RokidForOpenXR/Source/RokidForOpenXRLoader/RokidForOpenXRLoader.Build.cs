using UnrealBuildTool;
using System.IO;

public class RokidForOpenXRLoader : ModuleRules
{
    public RokidForOpenXRLoader(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "RokidForOpenXRLoader/Lib/arm64-v8a/libopenxr_loader.so"));
            // AndroidPlugin
            {
                string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
                AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "RokidMobile_APL.xml"));
            }
        }
    }
}