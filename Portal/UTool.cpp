// Fill out your copyright notice in the Description page of Project Settings.


#include "UTool.h"

UTool::UTool()
{
}

UTool::~UTool()
{
}

FVector UTool::ConvertLocationToActorSpace(FVector Location, AActor* Reference, AActor* Target)
{
	if (Reference == nullptr || Target == nullptr)
	{
		return FVector::ZeroVector;
	}

	FVector Direction = Location - Reference->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	FVector NewDirection = FVector::DotProduct(Direction, Reference->GetActorForwardVector()) * Target->GetActorForwardVector()
						+ FVector::DotProduct(Direction, Reference->GetActorRightVector()) * Target->GetActorRightVector()
						+ FVector::DotProduct(Direction, Reference->GetActorUpVector()) * Target->GetActorUpVector();
	FVector InverseNewDirection = FVector(-NewDirection.X, -NewDirection.Y, NewDirection.Z);
	return TargetLocation + InverseNewDirection;
	/*FVector NormalDirection = NewDirection;
	NormalDirection.Normalize();
	return TargetLocation + NewDirection + -2.f * NormalDirection;*/
}

FRotator UTool::ConvertRotationToActorSpace(FRotator Rotation, AActor* Reference, AActor* Target)
{
	if (Reference == nullptr || Target == nullptr)
	{
		return FRotator::ZeroRotator;
	}

	FTransform SourceTransform = Reference->GetActorTransform();
	FTransform TargetTransform = Target->GetActorTransform();
	FQuat QuatRotation = FQuat(Rotation);

	FQuat LocalQuat = SourceTransform.GetRotation().Inverse() * QuatRotation;
	FQuat NewWorldQuat = TargetTransform.GetRotation() * LocalQuat;
	FRotator InverseNewRotator = FRotator(
		NewWorldQuat.Rotator().Pitch,
		NewWorldQuat.Rotator().Yaw + 180,
		NewWorldQuat.Rotator().Roll
	);

	return InverseNewRotator.Clamp();
}

FVector UTool::ConvertVelocityToActorSpace(FVector Velocity, AActor* Reference, AActor* Target)
{
	if (Reference == nullptr || Target == nullptr)
	{
		return FVector::ZeroVector;
	}

	FVector NewVelocity = FVector::DotProduct(Velocity, Reference->GetActorForwardVector()) * Target->GetActorForwardVector()
		+ FVector::DotProduct(Velocity, Reference->GetActorRightVector()) * Target->GetActorRightVector()
		+ FVector::DotProduct(Velocity, Reference->GetActorUpVector()) * Target->GetActorUpVector();
	return NewVelocity * -1.f;
}

FVector UTool::ConvertCameraLocation(FVector Location, AActor* Reference, AActor* Target)
{
	if (Reference == nullptr || Target == nullptr)
	{
		return FVector::ZeroVector;
	}

	FVector Direction = Location - Reference->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	FVector NewDirection = FVector::DotProduct(Direction, Reference->GetActorForwardVector()) * Target->GetActorForwardVector()
		+ FVector::DotProduct(Direction, Reference->GetActorRightVector()) * Target->GetActorRightVector()
		+ FVector::DotProduct(Direction, Reference->GetActorUpVector()) * Target->GetActorUpVector();
	FVector InverseNewDirection = FVector(-NewDirection.X, -NewDirection.Y, NewDirection.Z);
	return TargetLocation + InverseNewDirection;
}
