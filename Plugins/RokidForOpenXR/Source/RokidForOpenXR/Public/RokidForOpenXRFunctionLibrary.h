// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RokidForOpenXRFunctionLibrary.generated.h"

UCLASS(ClassGroup = OpenXR)
class ROKIDFOROPENXR_API URokidForOpenXRFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Rokid OpenXR|Controller")
	static bool GetControllerPose(FVector& Position, FQuat& Rotation, FVector PositionOffset);
	
	UFUNCTION(BlueprintCallable, Category = "Rokid OpenXR|Controller")
	static void RecenterControllerPose();
	
	UFUNCTION(BlueprintPure, Category = "Rokid OpenXR|DeviceInfo")
	static void GetFocalLength(TArray<float>& data);
	
	UFUNCTION(BlueprintPure, Category = "Rokid OpenXR|DeviceInfo")
	static void GetPrincipalPoint(TArray<float>& data);
	
	UFUNCTION(BlueprintPure, Category = "Rokid OpenXR|DeviceInfo")
	static void GetImageDimensions(TArray<int32>& data);
	
	UFUNCTION(BlueprintPure, Category = "Rokid OpenXR|DeviceInfo")
	static void GetDistortion(TArray<float>& data);

	UFUNCTION(BlueprintCallable, Category = "Rokid OpenXR|DeviceInfo")
	static void GetHistoryCameraPhysicsPose(int64 timeStamp, FVector& Position, FQuat& Rotation);

	UFUNCTION(BlueprintPure, Category = "Rokid OpenXR|DeviceInfo")
	static void GetCameraPhysicsPose(int64& timeStamp, FVector& Position, FQuat& Rotation);

	UFUNCTION(BlueprintPure, Category = "Rokid OpenXR|DeviceInfo")
	static void GetSLAMQuality(int32& trackingStatus, int32& imageQuality, int32& kineticQuality);

	UFUNCTION(BlueprintPure, Category = "Rokid OpenXR|DeviceInfo")
	static void GetSlamState(int32& state);

	UFUNCTION(BlueprintPure, Category = "Rokid OpenXR|DeviceInfo")
	static void GetGlassName(FString& name);

	UFUNCTION(BlueprintPure, Category = "Rokid OpenXR|DeviceInfo")
	static void IsUsbConnect(bool& isConnect);
            
	UFUNCTION(BlueprintPure, Category = "Rokid OpenXR|DeviceInfo")
	static void GetGlassProductId(int32& pid);

	UFUNCTION(BlueprintPure, Category = "Rokid OpenXR|DeviceInfo")
	static void GetGlassTypeId(FString& typeId);
	
	UFUNCTION(BlueprintPure, Category = "Rokid OpenXR|DeviceInfo")
	static void GetGlassSeed(FString& seed);
            
	UFUNCTION(BlueprintPure, Category = "Rokid OpenXR|DeviceInfo")
	static void GetGlassSn(FString& sn);
    
	UFUNCTION(BlueprintPure, Category = "Rokid OpenXR|DeviceInfo")
	static void GetGlassFirmwareVersion(FString& version);

	static void OpenCameraPreview(PFN_xrCameraUpdateCallback callback);
	static void CloseCameraPreview();
	
	static XrResult RKOpenMarker(const void* database, uint32& size);
	static XrResult RKCloseMarker();
	static XrResult RKMarkerAcquireChanges(XrRokidMarker2ArrayExt* added, XrRokidMarker2ArrayExt* updated, XrRokidMarker2ArrayExt* removed);
	static XrResult RKMarkerAddImagePhy(const char* id, const uint8* imageData, int32& width, int32& height, int32& stride, float& widthInMeter, float& heightInMeter);
	static XrResult RKMarkerGetCenterPose(intptr_t marker, TArray<float>& out);
	static XrResult RKMarkerGetExtent(intptr_t marker, float& outX, float& outY);
	static XrResult RKMarkerGetId(intptr_t marker, char* id, int& idSize);
	static XrResult RKMarkerGetAlgoId(intptr_t marker, intptr_t& id);

	static XrResult RKOpenPlaneTracker(uint32 mode);
	static XrResult RKClosePlaneTracker();
	static XrResult RKGetPlaneDetectMode(uint32& mode);
	static XrResult RKSetPlaneDetectMode(uint32 mode);
	static XrResult RKReleaseMemory(void* point);
	static XrResult RKGetUpdatePlanes(uint32 type, void*& nochangePlanes, void*& changePlanes, void*& newPlanes, void*& removePlanes, uint32& nochangePlanesSize, uint32& changePlanesSize, uint32& newPlanesSize, uint32& removePlanesSize);
	static XrResult RKGetPlanePolygon(int64 plane, void*& polygon, uint32& polygonSize);
	static XrResult RKGetPlaneType(int64 plane, uint32& type);
	static XrResult RKGetPlaneCenterPose(int64 plane, void*& pose, void*& center, void*& normalVector);
	static XrResult RKGetPlaneRectangle(int64 plane, void*& rectangle);
	static XrResult RKReleasePlane(int64 plane);
	static XrResult RKCreateArtificialPlane(float& points, uint32& numPoints, float& width, float& height);
};
