#include "RokidForOpenXRPlugin.h"

#if PLATFORM_ANDROID
#include <dlfcn.h> 
#endif //PLATFORM_ANDROID

#define LOCTEXT_NAMESPACE "FRokidForOpenXRPlugin"

FRokidForOpenXRPlugin::FRokidForOpenXRPlugin()
	: LoaderHandle(nullptr)
	, bRokidRuntime(false)
{
}

void FRokidForOpenXRPlugin::Register()
{
	UE_LOG(LogTemp, Log, TEXT("FRokidForOpenXRPlugin Register"));
	RegisterOpenXRExtensionModularFeature();
}

void FRokidForOpenXRPlugin::Unregister()
{
	UE_LOG(LogTemp, Log, TEXT("FRokidForOpenXRPlugin Unregister"));
	UnregisterOpenXRExtensionModularFeature();
	if (LoaderHandle)
	{
		FPlatformProcess::FreeDllHandle(LoaderHandle);
		LoaderHandle = nullptr;
	}
}

bool FRokidForOpenXRPlugin::GetCustomLoader(PFN_xrGetInstanceProcAddr* OutGetProcAddr)
{
	UE_LOG(LogTemp, Log, TEXT("FRokidForOpenXRPlugin GetCustomLoader"));
	
#if PLATFORM_ANDROID
	
	// clear errors
	dlerror();

	LoaderHandle = FPlatformProcess::GetDllHandle(TEXT("libopenxr_loader.so"));
	if (LoaderHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to load rokid libopenxr_loader.so, error %s"), ANSI_TO_TCHAR(dlerror()));
		return false;
	}

	// clear errors
	dlerror();

	PFN_xrGetInstanceProcAddr xrGetInstanceProcAddrPtr = (PFN_xrGetInstanceProcAddr)FPlatformProcess::GetDllExport(LoaderHandle, TEXT("xrGetInstanceProcAddr"));
	if (xrGetInstanceProcAddrPtr == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to load OpenXR xrGetInstanceProcAddr, error %s"), ANSI_TO_TCHAR(dlerror()));
		return false;
	}
	*OutGetProcAddr = xrGetInstanceProcAddrPtr;

	extern struct android_app* GNativeAndroidApp;
	PFN_xrInitializeLoaderKHR xrInitializeLoaderKHR;
	xrGetInstanceProcAddrPtr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction*)&xrInitializeLoaderKHR);
	if (xrInitializeLoaderKHR == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to load OpenXR xrInitializeLoaderKHR"));
		return false;
	}
	XrLoaderInitInfoAndroidKHR LoaderInitializeInfoAndroid;
	LoaderInitializeInfoAndroid.type = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR;
	LoaderInitializeInfoAndroid.next = NULL;
	LoaderInitializeInfoAndroid.applicationVM = GNativeAndroidApp->activity->vm;
	LoaderInitializeInfoAndroid.applicationContext = GNativeAndroidApp->activity->clazz;
	XR_ENSURE(xrInitializeLoaderKHR((XrLoaderInitInfoBaseHeaderKHR*)&LoaderInitializeInfoAndroid));

	{
		PFN_xrEnumerateInstanceExtensionProperties xrEnumerateInstanceExtensionPropertiesPtr;
		xrGetInstanceProcAddrPtr(XR_NULL_HANDLE, "xrEnumerateInstanceExtensionProperties", (PFN_xrVoidFunction*)&xrEnumerateInstanceExtensionPropertiesPtr);
		if (xrEnumerateInstanceExtensionPropertiesPtr == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to load OpenXR xrEnumerateInstanceExtensionProperties!"));
			return false;
		}

		uint32_t ExtensionsCount = 0;
		if (XR_FAILED(xrEnumerateInstanceExtensionPropertiesPtr(nullptr, 0, &ExtensionsCount, nullptr)))
		{
			UE_LOG(LogTemp, Error, TEXT("xrEnumerateInstanceExtensionPropertiesPtr Failed!"));
			return false;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Loaded Rokid For OpenXR Loader"));
	bRokidRuntime = true;
	return true;
#endif //PLATFORM_ANDROID
	return false;
}

bool FRokidForOpenXRPlugin::GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions)
{
	UE_LOG(LogTemp, Log, TEXT("FRokidForOpenXRPlugin GetOptionalExtensions()"));
	return bRokidRuntime;
}

void FRokidForOpenXRPlugin::PostCreateInstance(XrInstance InInstance)
{
	XrInstanceProperties InstanceProps = { XR_TYPE_INSTANCE_PROPERTIES, nullptr };
	XR_ENSURE(xrGetInstanceProperties(InInstance, &InstanceProps));
	InstanceProps.runtimeName[XR_MAX_RUNTIME_NAME_SIZE - 1] = 0;
	FString RuntimeName = UTF8_TO_TCHAR(InstanceProps.runtimeName);
	UE_LOG(LogTemp, Log, TEXT("FRokidForOpenXRPlugin PostCreateInstance RuntimeName:%s"), *RuntimeName);
}
	
const void* FRokidForOpenXRPlugin::OnCreateSession(XrInstance InInstance, XrSystemId InSystem, const void* InNext)
{
#if PLATFORM_ANDROID
	UE_LOG(LogTemp, Log, TEXT("FRokidForOpenXRPlugin OnCreateSession()"));
	if (GRHISupportsRHIThread && GIsThreadedRendering && GUseRHIThread_InternalUseOnly)
	{
		SetRHIThreadEnabled(false, false);
	}

	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetFocalLength", (PFN_xrVoidFunction*)&xrGetFocalLength));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetPrincipalPoint", (PFN_xrVoidFunction*)&xrGetPrincipalPoint));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetImageDimensions", (PFN_xrVoidFunction*)&xrGetImageDimensions));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetDistortion", (PFN_xrVoidFunction*)&xrGetDistortion));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetHistoryCameraPhysicsPose", (PFN_xrVoidFunction*)&xrGetHistoryCameraPhysicsPose));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetCameraPhysicsPose", (PFN_xrVoidFunction*)&xrGetCameraPhysicsPose));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetSLAMQuality", (PFN_xrVoidFunction*)&xrGetSLAMQuality));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetHeadTrackingStatus", (PFN_xrVoidFunction*)&xrGetSlamState));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetGlassName", (PFN_xrVoidFunction*)&xrGetGlassName));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrIsUsbConnect", (PFN_xrVoidFunction*)&xrIsUsbConnect));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetGlassProductId", (PFN_xrVoidFunction*)&xrGetGlassProductId));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetGlassTypeId", (PFN_xrVoidFunction*)&xrGetGlassTypeId));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetGlassSeed", (PFN_xrVoidFunction*)&xrGetGlassSeed));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetGlassSn", (PFN_xrVoidFunction*)&xrGetGlassSn));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetGlassFirmwareVersion", (PFN_xrVoidFunction*)&xrGetGlassFirmwareVersion));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrOpenCameraPreview", (PFN_xrVoidFunction*)&xrOpenCameraPreview));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrCloseCameraPreview", (PFN_xrVoidFunction*)&xrCloseCameraPreview));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKOpenMarker2", (PFN_xrVoidFunction*)&xrRKOpenMarker2));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKCloseMarker2", (PFN_xrVoidFunction*)&xrRKCloseMarker2));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKMarker2AcquireChanges", (PFN_xrVoidFunction*)&xrRKMarker2AcquireChanges));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKMarker2AddImagePhy", (PFN_xrVoidFunction*)&xrRKMarker2AddImagePhy));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKMarker2GetCenterPose", (PFN_xrVoidFunction*)&xrRKMarker2GetCenterPose));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKMarker2GetExtent", (PFN_xrVoidFunction*)&xrRKMarker2GetExtent));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKMarker2GetId", (PFN_xrVoidFunction*)&xrRKMarker2GetId));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKMarker2GetAlgoId", (PFN_xrVoidFunction*)&xrRKMarker2GetAlgoId));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKOpenPlaneTracker", (PFN_xrVoidFunction*)&xrRKOpenPlaneTracker));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKClosePlaneTracker", (PFN_xrVoidFunction*)&xrRKClosePlaneTracker));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKGetPlaneDetectMode", (PFN_xrVoidFunction*)&xrRKGetPlaneDetectMode));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKSetPlaneDetectMode", (PFN_xrVoidFunction*)&xrRKSetPlaneDetectMode));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKReleaseMemory", (PFN_xrVoidFunction*)&xrRKReleaseMemory));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKGetUpdatePlanes", (PFN_xrVoidFunction*)&xrRKGetUpdatePlanes));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKGetPlanePolygon", (PFN_xrVoidFunction*)&xrRKGetPlanePolygon));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKGetPlaneType", (PFN_xrVoidFunction*)&xrRKGetPlaneType));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKGetPlaneCenterPose", (PFN_xrVoidFunction*)&xrRKGetPlaneCenterPose));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKGetPlaneRectangle", (PFN_xrVoidFunction*)&xrRKGetPlaneRectangle));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKReleasePlane", (PFN_xrVoidFunction*)&xrRKReleasePlane));
	XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrRKCreateArtificialPlane", (PFN_xrVoidFunction*)&xrRKCreateArtificialPlane));
#endif // PLATFORM_ANDROID

	return InNext;
}

void FRokidForOpenXRPlugin::GetFocalLength(TArray<float>& data)
{
	if (xrGetFocalLength)
	{
		float _data[2];
		xrGetFocalLength(_data);
		data.Add(_data[0]);
		data.Add(_data[1]);
	}
}

void FRokidForOpenXRPlugin::GetPrincipalPoint(TArray<float>& data)
{
	if (xrGetPrincipalPoint)
	{
		float _data[2];
		xrGetPrincipalPoint(_data);
		data.Add(_data[0]);
		data.Add(_data[1]);
	}
}

void FRokidForOpenXRPlugin::GetImageDimensions(TArray<int32>& data)
{
	if (xrGetImageDimensions)
	{
		int _data[2];
		xrGetImageDimensions(_data);
		data.Add(_data[0]);
		data.Add(_data[1]);
	}
}

void FRokidForOpenXRPlugin::GetDistortion(TArray<float>& data)
{
	if (xrGetDistortion)
	{
		float _data[5];
		xrGetDistortion(_data);
		data.Add(_data[0]);
		data.Add(_data[1]);
		data.Add(_data[2]);
		data.Add(_data[3]);
		data.Add(_data[4]);
	}
}

void FRokidForOpenXRPlugin::GetHistoryCameraPhysicsPose(int64 timeStamp, FVector& Position, FQuat& Rotation)
{
	if (xrGetHistoryCameraPhysicsPose)
	{
		float position[3];
		float orientation[4];
		xrGetHistoryCameraPhysicsPose(timeStamp, position, orientation);
		Position.X = position[0];
		Position.Y = position[1];
		Position.Z = position[2];
		Rotation.X = orientation[0];
		Rotation.Y = orientation[1];
		Rotation.Z = orientation[2];
		Rotation.W = orientation[3];
	}
}

void FRokidForOpenXRPlugin::GetCameraPhysicsPose(int64& timeStamp, FVector& Position, FQuat& Rotation)
{
	if (xrGetCameraPhysicsPose)
	{
		float position[3];
		float orientation[4];
		xrGetCameraPhysicsPose(reinterpret_cast<uint64_t*>(&timeStamp), position, orientation);
		Position.X = position[0];
		Position.Y = position[1];
		Position.Z = position[2];
		Rotation.X = orientation[0];
		Rotation.Y = orientation[1];
		Rotation.Z = orientation[2];
		Rotation.W = orientation[3];
	}
}

void FRokidForOpenXRPlugin::GetSLAMQuality(int32& trackingStatus, int32& imageQuality, int32& kineticQuality)
{
	if (xrGetSLAMQuality)
	{
		xrGetSLAMQuality(reinterpret_cast<uint32_t*>(&trackingStatus), reinterpret_cast<uint32_t*>(&imageQuality), reinterpret_cast<uint32_t*>(&kineticQuality));
	}
}

void FRokidForOpenXRPlugin::GetSlamState(int32& state)
{
	if (xrGetSlamState)
	{
		xrGetSlamState(reinterpret_cast<uint32_t*>(&state));
	}
}

void FRokidForOpenXRPlugin::GetGlassName(FString& name)
{
	if (xrGetGlassName)
	{
		char* dataStr = new char[50];
		xrGetGlassName(dataStr, 50);
		name = FString(dataStr);
	}
}

void FRokidForOpenXRPlugin::IsUsbConnect(bool& isConnect)
{
	if (xrIsUsbConnect)
	{
		xrIsUsbConnect(reinterpret_cast<uint32_t*>(&isConnect));
	}
}

void FRokidForOpenXRPlugin::GetGlassProductId(int32& pid)
{
	if (xrGetGlassProductId)
	{
		xrGetGlassProductId(&pid);
	}
}

void FRokidForOpenXRPlugin::GetGlassTypeId(FString& typeId)
{
	if (xrGetGlassTypeId)
	{
		char* dataStr = new char[50];
		xrGetGlassTypeId(dataStr, 50);
		typeId = FString(dataStr);
	}
}

void FRokidForOpenXRPlugin::GetGlassSeed(FString& seed)
{
	if (xrGetGlassSeed)
	{
		char* dataStr = new char[50];
		xrGetGlassSeed(dataStr, 50);
		seed = FString(dataStr);
	}
}

void FRokidForOpenXRPlugin::GetGlassSn(FString& sn)
{
	if (xrGetGlassSn)
	{
		char* dataStr = new char[50];
		xrGetGlassSn(dataStr, 50);
		sn = FString(dataStr);
	}
}

void FRokidForOpenXRPlugin::GetGlassFirmwareVersion(FString& version)
{
	if (xrGetGlassFirmwareVersion)
	{
		char* dataStr = new char[50];
		xrGetGlassFirmwareVersion(dataStr, 50);
		version = FString(dataStr);
	}
}

void FRokidForOpenXRPlugin::OpenCameraPreview(PFN_xrCameraUpdateCallback callback)
{
	if (xrOpenCameraPreview)
	{
		xrOpenCameraPreview(callback);
	}
}

void FRokidForOpenXRPlugin::CloseCameraPreview()
{
	if (xrCloseCameraPreview)
	{
		xrCloseCameraPreview();
	}
}

XrResult FRokidForOpenXRPlugin::RKOpenMarker(const void* database, uint32& size)
{
	if (xrRKOpenMarker2)
	{
		XrResult xrResult = xrRKOpenMarker2(database, size);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKCloseMarker()
{
	if (xrRKCloseMarker2)
	{
		XrResult xrResult = xrRKCloseMarker2();
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}


XrResult FRokidForOpenXRPlugin::RKMarkerAcquireChanges(XrRokidMarker2ArrayExt* added, XrRokidMarker2ArrayExt* updated, XrRokidMarker2ArrayExt* removed)
{
	if (xrRKMarker2AcquireChanges)
	{
		XrResult xrResult = xrRKMarker2AcquireChanges(added, updated, removed);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKMarkerAddImagePhy(const char* id, const uint8* imageData, int32& width, int32& height, int32& stride, float& widthInMeter, float& heightInMeter)
{
	if (xrRKMarker2AddImagePhy)
	{
		XrResult xrResult = xrRKMarker2AddImagePhy(id, imageData, width, height, stride, widthInMeter, heightInMeter);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKMarkerGetCenterPose(intptr_t marker, TArray<float>& out)
{
	if (xrRKMarker2AddImagePhy)
	{
		float _out[7];
		XrResult xrResult = xrRKMarker2GetCenterPose(marker, _out);
		out.Add(_out[0]);
		out.Add(_out[1]);
		out.Add(_out[2]);
		out.Add(_out[3]);
		out.Add(_out[4]);
		out.Add(_out[5]);
		out.Add(_out[6]);

		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKMarkerGetExtent(intptr_t marker, float& outX, float& outY)
{
	if (xrRKMarker2GetExtent)
	{
		XrResult xrResult = xrRKMarker2GetExtent(marker, &outX, &outY);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKMarkerGetId(intptr_t marker, char* id, int& idSize)
{
	if (xrRKMarker2GetId)
	{
		XrResult xrResult = xrRKMarker2GetId(marker, id, idSize);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKMarkerGetAlgoId(intptr_t marker, intptr_t& id)
{
	if (xrRKMarker2GetAlgoId)
	{
		XrResult xrResult = xrRKMarker2GetAlgoId(marker, &id);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKOpenPlaneTracker(uint32 mode)
{
	if (xrRKOpenPlaneTracker)
	{
		XrResult xrResult = xrRKOpenPlaneTracker(mode);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKClosePlaneTracker()
{
	if (xrRKClosePlaneTracker)
	{
		XrResult xrResult = xrRKClosePlaneTracker();
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKGetPlaneDetectMode(uint32& mode)
{
	if (xrRKGetPlaneDetectMode)
	{ 
		XrResult xrResult = xrRKGetPlaneDetectMode(&mode);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}


XrResult FRokidForOpenXRPlugin::RKSetPlaneDetectMode(uint32 mode)
{
	if (xrRKSetPlaneDetectMode)
	{
		XrResult xrResult = xrRKSetPlaneDetectMode(mode);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKReleaseMemory(void* point)
{
	if (xrRKReleaseMemory)
	{
		XrResult xrResult = xrRKReleaseMemory(point);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKGetUpdatePlanes(uint32 type, void*& nochangePlanes, void*& changePlanes, void*& newPlanes, void*& removePlanes, uint32& nochangePlanesSize, uint32& changePlanesSize, uint32& newPlanesSize, uint32& removePlanesSize)
{
	if (xrRKGetUpdatePlanes)
	{
		XrResult xrResult = xrRKGetUpdatePlanes(type, &nochangePlanes, &changePlanes, &newPlanes, &removePlanes, &nochangePlanesSize, &changePlanesSize, &newPlanesSize, &removePlanesSize);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKGetPlanePolygon(int64 plane, void*& polygon, uint32& polygonSize)
{
	if (xrRKGetPlanePolygon)
	{
		XrResult xrResult = xrRKGetPlanePolygon(plane, &polygon, &polygonSize);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKGetPlaneType(int64 plane, uint32& type)
{
	if (xrRKGetPlaneType)
	{
		XrResult xrResult = xrRKGetPlaneType(plane, &type);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKGetPlaneCenterPose(int64 plane, void*& pose, void*& center, void*& normalVector)
{
	if (xrRKGetPlaneCenterPose)
	{
		XrResult xrResult = xrRKGetPlaneCenterPose(plane, &pose, &center, &normalVector);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKGetPlaneRectangle(int64 plane, void*& rectangle)
{
	if (xrRKGetPlaneRectangle)
	{
		XrResult xrResult = xrRKGetPlaneRectangle(plane,&rectangle);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKReleasePlane(int64 plane)
{
	if (xrRKReleasePlane)
	{
		XrResult xrResult = xrRKReleasePlane(plane);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}

XrResult FRokidForOpenXRPlugin::RKCreateArtificialPlane(float& points, uint32& numPoints, float& width, float& height)
{
	if (xrRKCreateArtificialPlane)
	{
		XrResult xrResult = xrRKCreateArtificialPlane(&points, numPoints, width, height);
		return xrResult;
	}
	else
	{
		return XR_ERROR_INITIALIZATION_FAILED;
	}
}