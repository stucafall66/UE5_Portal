// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalLocalPlayer.h"

UPortalLocalPlayer::UPortalLocalPlayer()
{
	bCameraCut = false;
}

FSceneView* UPortalLocalPlayer::CalcSceneView(FSceneViewFamily* ViewFamily, FVector& OutViewLocation, FRotator& OutViewRotation, FViewport* Viewport, FViewElementDrawer* ViewDrawer, int32 StereoViewIndex)
{
	FSceneView* View = Super::CalcSceneView(ViewFamily, OutViewLocation, OutViewRotation, Viewport, ViewDrawer, StereoViewIndex);
	if (bCameraCut)
	{
		View->bCameraCut = true;
		bCameraCut = false;
	}

	return View;
}

void UPortalLocalPlayer::PerformCameraCut()
{
	bCameraCut = true;
}
