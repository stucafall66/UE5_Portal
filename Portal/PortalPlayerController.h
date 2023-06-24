// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PortalPlayerController.generated.h"

/**
 * 
 */
class APortalManager;
class APortalCharacter;
class UPortalLocalPlayer;

UCLASS()
class PORTAL_API APortalPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APortalCharacter* GetCharacter() const;

	FMatrix GetCameraProjectionMatrix();

	void PerformCameraCut();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UPortalLocalPlayer* LocalPlayer;

};
