#pragma once

#include "HAL/Platform.h"
#if PLATFORM_ANDROID
#define XR_USE_PLATFORM_ANDROID 1
#include <android_native_app_glue.h>
#endif

#include <ThirdParty/openxr/openxr.h>
#include <ThirdParty/openxr/openxr_platform.h>
#include <ThirdParty/openxr/openxr_reflection.h>

#include "OpenXRCore.h"
#include "OpenXRPlatformRHI.h"

#include "IOpenXRHMDModule.h"
#include "IOpenXRExtensionPlugin.h"
