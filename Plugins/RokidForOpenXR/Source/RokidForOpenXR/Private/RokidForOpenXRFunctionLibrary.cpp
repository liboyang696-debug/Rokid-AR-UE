// Fill out your copyright notice in the Description page of Project Settings.

#include "RokidForOpenXRFunctionLibrary.h"

#include "RokidForOpenXR.h"

bool URokidForOpenXRFunctionLibrary::GetControllerPose(FVector& Position, FQuat& Rotation, FVector PositionOffset)
{
	return FRokidForOpenXRModule::Get().GetControllerPlugin().GetControllerPose(Position, Rotation, PositionOffset);
}

void URokidForOpenXRFunctionLibrary::RecenterControllerPose()
{
	FRokidForOpenXRModule::Get().GetControllerPlugin().RecenterControllerPose();
}
	
void URokidForOpenXRFunctionLibrary::GetFocalLength(TArray<float>& data)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().GetFocalLength(data);
}
	
void URokidForOpenXRFunctionLibrary::GetPrincipalPoint(TArray<float>& data)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().GetPrincipalPoint(data);
}
	
void URokidForOpenXRFunctionLibrary::GetImageDimensions(TArray<int32>& data)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().GetImageDimensions(data);
}

void URokidForOpenXRFunctionLibrary::GetDistortion(TArray<float>& data)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().GetDistortion(data);
}

void URokidForOpenXRFunctionLibrary::GetHistoryCameraPhysicsPose(int64 timeStamp, FVector& Position, FQuat& Rotation)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().GetHistoryCameraPhysicsPose(timeStamp, Position, Rotation);
}

void URokidForOpenXRFunctionLibrary::GetCameraPhysicsPose(int64& timeStamp, FVector& Position, FQuat& Rotation)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().GetCameraPhysicsPose(timeStamp, Position, Rotation);
}

void URokidForOpenXRFunctionLibrary::GetSLAMQuality(int32& trackingStatus, int32& imageQuality, int32& kineticQuality)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().GetSLAMQuality(trackingStatus, imageQuality, kineticQuality);
}

void URokidForOpenXRFunctionLibrary::GetSlamState(int32& state)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().GetSlamState(state);
}

void URokidForOpenXRFunctionLibrary::GetGlassName(FString& name)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().GetGlassName(name);
}

void URokidForOpenXRFunctionLibrary::IsUsbConnect(bool& isConnect)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().IsUsbConnect(isConnect);
}
        
void URokidForOpenXRFunctionLibrary::GetGlassProductId(int32& pid)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().GetGlassProductId(pid);
}

void URokidForOpenXRFunctionLibrary::GetGlassTypeId(FString& typeId)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().GetGlassTypeId(typeId);
}
	
void URokidForOpenXRFunctionLibrary::GetGlassSeed(FString& seed)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().GetGlassSeed(seed);
}
          
void URokidForOpenXRFunctionLibrary::GetGlassSn(FString& sn)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().GetGlassSn(sn);
}
  
void URokidForOpenXRFunctionLibrary::GetGlassFirmwareVersion(FString& version)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().GetGlassFirmwareVersion(version);
}

void URokidForOpenXRFunctionLibrary::OpenCameraPreview(PFN_xrCameraUpdateCallback callback)
{
	FRokidForOpenXRModule::Get().GetXRPlugin().OpenCameraPreview(callback);
}

void URokidForOpenXRFunctionLibrary::CloseCameraPreview()
{
	FRokidForOpenXRModule::Get().GetXRPlugin().CloseCameraPreview();
}


XrResult URokidForOpenXRFunctionLibrary::RKOpenMarker(const void* database, uint32& size)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKOpenMarker(database, size);
}

XrResult URokidForOpenXRFunctionLibrary::RKCloseMarker()
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKCloseMarker();
}

XrResult URokidForOpenXRFunctionLibrary::RKMarkerAcquireChanges(XrRokidMarker2ArrayExt* added, XrRokidMarker2ArrayExt* updated, XrRokidMarker2ArrayExt* removed)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKMarkerAcquireChanges(added, updated, removed);
}

XrResult URokidForOpenXRFunctionLibrary::RKMarkerAddImagePhy(const char* id, const uint8* imageData, int32& width, int32& height, int32& stride, float& widthInMeter, float& heightInMeter)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKMarkerAddImagePhy(id, imageData, width, height, stride, widthInMeter, heightInMeter);
}

XrResult URokidForOpenXRFunctionLibrary::RKMarkerGetCenterPose(intptr_t marker, TArray<float>& out)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKMarkerGetCenterPose(marker,out);
}

XrResult URokidForOpenXRFunctionLibrary::RKMarkerGetExtent(intptr_t marker, float& outX, float& outY)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKMarkerGetExtent(marker, outX, outY);
}

XrResult URokidForOpenXRFunctionLibrary::RKMarkerGetId(intptr_t marker, char* id, int& idSize)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKMarkerGetId(marker, id, idSize);
}

XrResult URokidForOpenXRFunctionLibrary::RKMarkerGetAlgoId(intptr_t marker, intptr_t& id)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKMarkerGetAlgoId(marker,  id);
}

XrResult URokidForOpenXRFunctionLibrary::RKOpenPlaneTracker(uint32 mode)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKOpenPlaneTracker(mode);
}

XrResult URokidForOpenXRFunctionLibrary::RKClosePlaneTracker()
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKClosePlaneTracker();
}

XrResult URokidForOpenXRFunctionLibrary::RKGetPlaneDetectMode(uint32& mode)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKGetPlaneDetectMode(mode);
}

XrResult URokidForOpenXRFunctionLibrary::RKSetPlaneDetectMode(uint32 mode)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKSetPlaneDetectMode(mode);
}

XrResult URokidForOpenXRFunctionLibrary::RKReleaseMemory(void* point)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKReleaseMemory(point);
}

XrResult URokidForOpenXRFunctionLibrary::RKGetUpdatePlanes(uint32 type, void*& nochangePlanes, void*& changePlanes, void*& newPlanes, void*& removePlanes, uint32& nochangePlanesSize, uint32& changePlanesSize, uint32& newPlanesSize, uint32& removePlanesSize)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKGetUpdatePlanes(type, nochangePlanes, changePlanes, newPlanes, removePlanes, nochangePlanesSize, changePlanesSize, newPlanesSize, removePlanesSize);
}

XrResult URokidForOpenXRFunctionLibrary::RKGetPlanePolygon(int64 plane, void*& polygon, uint32& polygonSize)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKGetPlanePolygon(plane, polygon, polygonSize);
}

XrResult URokidForOpenXRFunctionLibrary::RKGetPlaneType(int64 plane, uint32& type)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKGetPlaneType(plane, type);
}

XrResult URokidForOpenXRFunctionLibrary::RKGetPlaneCenterPose(int64 plane, void*& pose, void*& center, void*& normalVector)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKGetPlaneCenterPose(plane, pose, center, normalVector);
}

XrResult URokidForOpenXRFunctionLibrary::RKGetPlaneRectangle(int64 plane, void*& rectangle)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKGetPlaneRectangle(plane, rectangle);
}

XrResult URokidForOpenXRFunctionLibrary::RKReleasePlane(int64 plane)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKReleasePlane(plane);
}

XrResult URokidForOpenXRFunctionLibrary::RKCreateArtificialPlane(float& points, uint32& numPoints, float& width, float& height)
{
	return FRokidForOpenXRModule::Get().GetXRPlugin().RKCreateArtificialPlane(points, numPoints, width, height);
}