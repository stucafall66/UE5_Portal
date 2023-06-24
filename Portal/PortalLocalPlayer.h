// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "PortalLocalPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PORTAL_API UPortalLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:
	UPortalLocalPlayer();
	virtual FSceneView* CalcSceneView(class FSceneViewFamily* ViewFamily,
		FVector& OutViewLocation,
		FRotator& OutViewRotation,
		FViewport* Viewport,
		class FViewElementDrawer* ViewDrawer = NULL,
		int32 StereoViewIndex = INDEX_NONE) override;
	void PerformCameraCut();

private:
	bool bCameraCut;

};
