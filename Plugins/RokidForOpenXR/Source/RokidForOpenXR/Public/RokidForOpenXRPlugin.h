#pragma once

class FRokidForOpenXRPlugin : public IOpenXRExtensionPlugin
{
public:
	FRokidForOpenXRPlugin();
	virtual ~FRokidForOpenXRPlugin() {}

	void Register();
	void Unregister();

	virtual FString GetDisplayName() override
	{
		return FString(TEXT("RokidForOpenXRPlugin"));
	}

	virtual bool GetCustomLoader(PFN_xrGetInstanceProcAddr* OutGetProcAddr) override;
	virtual bool GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
	virtual void PostCreateInstance(XrInstance InInstance) override;
	virtual const void* OnCreateSession(XrInstance InInstance, XrSystemId InSystem, const void* InNext) override;

	void GetFocalLength(TArray<float>& data);
	void GetPrincipalPoint(TArray<float>& data);
	void GetImageDimensions(TArray<int32>& data);
	void GetDistortion(TArray<float>& data);
	void GetHistoryCameraPhysicsPose(int64 timeStamp, FVector& Position, FQuat& Rotation);
	void GetCameraPhysicsPose(int64& timeStamp, FVector& Position, FQuat& Rotation);
	void GetSLAMQuality(int32& trackingStatus, int32& imageQuality, int32& kineticQuality);
	void GetSlamState(int32& state);
	void GetGlassName(FString& name);
	void IsUsbConnect(bool& isConnect);
	void GetGlassProductId(int32& pid);
	void GetGlassTypeId(FString& typeId);
	void GetGlassSeed(FString& seed);
	void GetGlassSn(FString& sn);
	void GetGlassFirmwareVersion(FString& version);
    
	void OpenCameraPreview(PFN_xrCameraUpdateCallback callback);
	void CloseCameraPreview();

	XrResult RKOpenMarker(const void* database, uint32& size);
	XrResult RKCloseMarker();
	XrResult RKMarkerAcquireChanges(XrRokidMarker2ArrayExt* added, XrRokidMarker2ArrayExt* updated, XrRokidMarker2ArrayExt* removed);
	XrResult RKMarkerAddImagePhy(const char* id, const uint8* imageData, int32& width, int32& height, int32& stride, float& widthInMeter, float& heightInMeter);
	XrResult RKMarkerGetCenterPose(intptr_t marker, TArray<float>& out);
	XrResult RKMarkerGetExtent(intptr_t marker, float& outX, float& outY);
	XrResult RKMarkerGetId(intptr_t marker, char* id, int& idSize);
	XrResult RKMarkerGetAlgoId(intptr_t marker, intptr_t& id);

	XrResult RKOpenPlaneTracker(uint32 mode);
	XrResult RKClosePlaneTracker();
	XrResult RKGetPlaneDetectMode(uint32& mode);
	XrResult RKSetPlaneDetectMode(uint32 mode);
	XrResult RKReleaseMemory(void* point);
	XrResult RKGetUpdatePlanes(uint32 type, void*& nochangePlanes, void*& changePlanes, void*& newPlanes, void*& removePlanes, uint32& nochangePlanesSize, uint32& changePlanesSize, uint32& newPlanesSize, uint32& removePlanesSize);
	XrResult RKGetPlanePolygon(int64 plane, void*& polygon, uint32& polygonSize);
	XrResult RKGetPlaneType(int64 plane, uint32& type);
	XrResult RKGetPlaneCenterPose(int64 plane, void*& pose, void*& center, void*& normalVector);
	XrResult RKGetPlaneRectangle(int64 plane, void*& rectangle);
	XrResult RKReleasePlane(int64 plane);
	XrResult RKCreateArtificialPlane(float& points, uint32& numPoints, float& width, float& height);


private:
	void*								 LoaderHandle;
	bool								 bRokidRuntime;

	PFN_xrGetFocalLength xrGetFocalLength;
	PFN_xrGetPrincipalPoint xrGetPrincipalPoint;
	PFN_xrGetImageDimensions xrGetImageDimensions;
	PFN_xrGetDistortion xrGetDistortion;
	PFN_xrGetHistoryCameraPhysicsPose xrGetHistoryCameraPhysicsPose;
	PFN_xrGetCameraPhysicsPose xrGetCameraPhysicsPose;
	PFN_xrGetSLAMQuality xrGetSLAMQuality;
	PFN_xrGetHeadTrackingStatus xrGetSlamState;
	PFN_xrGetGlassName xrGetGlassName;
	PFN_xrIsUsbConnect xrIsUsbConnect;
	PFN_xrGetGlassProductId xrGetGlassProductId;
	PFN_xrGetGlassTypeId xrGetGlassTypeId;
	PFN_xrGetGlassSeed xrGetGlassSeed;
	PFN_xrGetGlassSn xrGetGlassSn;
	PFN_xrGetGlassFirmwareVersion xrGetGlassFirmwareVersion;
	
	// Camera
	PFN_xrOpenCameraPreview xrOpenCameraPreview;
	PFN_xrCloseCameraPreview xrCloseCameraPreview;

	// Marker
	PFN_xrRKOpenMarker2 xrRKOpenMarker2;
	PFN_xrRKCloseMarker2 xrRKCloseMarker2;
	PFN_xrRKMarker2AcquireChanges xrRKMarker2AcquireChanges;
	PFN_xrRKMarker2AddImagePhy xrRKMarker2AddImagePhy;
	PFN_xrRKMarker2GetCenterPose xrRKMarker2GetCenterPose;
	PFN_xrRKMarker2GetExtent xrRKMarker2GetExtent;
	PFN_xrRKMarker2GetId xrRKMarker2GetId;
	PFN_xrRKMarker2GetAlgoId xrRKMarker2GetAlgoId;

	//PlaneTrack
	PFN_xrRKOpenPlaneTracker xrRKOpenPlaneTracker;
	PFN_xrRKClosePlaneTracker xrRKClosePlaneTracker;
	PFN_xrRKGetPlaneDetectMode xrRKGetPlaneDetectMode;
	PFN_xrRKSetPlaneDetectMode xrRKSetPlaneDetectMode;
	PFN_xrRKReleaseMemory xrRKReleaseMemory;

	PFN_xrRKGetUpdatePlanes xrRKGetUpdatePlanes;
	PFN_xrRKGetPlanePolygon xrRKGetPlanePolygon;
	PFN_xrRKGetPlaneType xrRKGetPlaneType;
	PFN_xrRKGetPlaneCenterPose xrRKGetPlaneCenterPose;
	PFN_xrRKGetPlaneRectangle xrRKGetPlaneRectangle;
	PFN_xrRKReleasePlane xrRKReleasePlane;
	PFN_xrRKCreateArtificialPlane xrRKCreateArtificialPlane;
};
