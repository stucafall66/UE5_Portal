// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PORTAL_API UTool
{
public:
	UTool();
	~UTool();

	static FVector ConvertLocationToActorSpace(FVector Location, AActor* Reference, AActor* Target);
	static FRotator ConvertRotationToActorSpace(FRotator Rotation, AActor* Reference, AActor* Target);
	static FVector ConvertVelocityToActorSpace(FVector Velocity, AActor* Reference, AActor* Target);
	static FVector ConvertCameraLocation(FVector Location, AActor* Reference, AActor* Target);
};
