// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class RokidForOpenXR : ModuleRules
{
	public RokidForOpenXR(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = @"Private\OpenXRCommon.h";
		
		PrivateIncludePaths.AddRange(
            new string[]
            {
                "ThirdParty"
            });

        string EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
        PrivateIncludePaths.AddRange(
            new string[]
            {
                EngineDir+"/Plugins/Runtime/OpenXR/Source/OpenXRHMD/Private",
                EngineDir+"/Source/Runtime/Renderer/Private",
                EngineDir+"/Source/Runtime/OpenGLDrv/Private",
                EngineDir+"/Source/Runtime/VulkanRHI/Private",
                EngineDir+"/Source/Runtime/Engine/Classes/Components",
                EngineDir+"/Source/Runtime/Engine/Classes/Kismet",
            });

        PublicIncludePathModuleNames.AddRange(
            new string[]
            {
                "Launch",
                "OpenXRHMD",
            });

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PrivateIncludePaths.Add(EngineDir + "/Source/Runtime/VulkanRHI/Private/Windows");
        }
        else
        {
            PrivateIncludePaths.Add(EngineDir + "/Source/Runtime/VulkanRHI/Private/" + Target.Platform);
        }

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "LiveLink",
                "RHI",
                "RenderCore",
                "Renderer",
                "Slate",
                "SlateCore",
                "ImageWrapper",
                "MediaAssets",
                "Analytics",
                "OpenGLDrv",
                "VulkanRHI",
                "LiveLinkAnimationCore",
                "LiveLinkInterface",
                "OpenXRHMD",
                "OpenXRInput",
                "HeadMountedDisplay",
                "XRBase",
                "ApplicationCore",
                "RokidForOpenXRLoader",
                "Projects",
            });

        if (Target.Version.MajorVersion >= 5)
        {
            PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "RHICore",
            });
        }

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }

        AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenGL");

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // D3D
            {
                PrivateDependencyModuleNames.AddRange(
                    new string[]
                    {
                        "D3D11RHI",
                        "D3D12RHI",
                    });

                PrivateIncludePaths.AddRange(
                    new string[]
                    {
                        EngineDir+"/Source/Runtime/Windows/D3D11RHI/Private",
                        EngineDir+"/Source/Runtime/Windows/D3D11RHI/Private/Windows",
                        EngineDir+"/Source/Runtime/D3D12RHI/Private",
                        EngineDir+"/Source/Runtime/D3D12RHI/Private/Windows",
                    });

                AddEngineThirdPartyPrivateStaticDependencies(Target, "DX11");
                AddEngineThirdPartyPrivateStaticDependencies(Target, "DX12");
                AddEngineThirdPartyPrivateStaticDependencies(Target, "NVAPI");
                AddEngineThirdPartyPrivateStaticDependencies(Target, "DX11Audio");
                AddEngineThirdPartyPrivateStaticDependencies(Target, "DirectSound");
                AddEngineThirdPartyPrivateStaticDependencies(Target, "NVAftermath");
                AddEngineThirdPartyPrivateStaticDependencies(Target, "IntelMetricsDiscovery");
                AddEngineThirdPartyPrivateStaticDependencies(Target, "IntelExtensionsFramework");
            }

            // Vulkan
            {
                AddEngineThirdPartyPrivateStaticDependencies(Target, "Vulkan");
            }
        }
        else if (Target.Platform == UnrealTargetPlatform.Android)
        {
            // Vulkan
            {
                AddEngineThirdPartyPrivateStaticDependencies(Target, "Vulkan");
            }
        }
	}
}
