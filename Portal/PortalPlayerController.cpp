// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalPlayerController.h"
#include "PortalCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "PortalActor.h"
#include "EngineUtils.h"
#include "PortalLocalPlayer.h"

void APortalPlayerController::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerController Begin Play"));
	// Get All PortalActors, then set ControllerOwners for them
	for (TActorIterator<APortalActor>ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		APortalActor* Portal = *ActorItr;
		Portal->SetControllerOwner(this);
	}

	LocalPlayer = Cast<UPortalLocalPlayer>(GetLocalPlayer());
}

APortalCharacter* APortalPlayerController::GetCharacter() const
{
	APortalCharacter* PortalCharacter = nullptr;
	PortalCharacter = Cast<APortalCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	
	return PortalCharacter;
}

FMatrix APortalPlayerController::GetCameraProjectionMatrix()
{
	FMatrix ProjectionMatrix;

	if (GetLocalPlayer() != nullptr)
	{
		FSceneViewProjectionData PlayerProjectionData;

		GetLocalPlayer()->GetProjectionData(GetLocalPlayer()->ViewportClient->Viewport,
			PlayerProjectionData);

		ProjectionMatrix = PlayerProjectionData.ProjectionMatrix;
	}

	return ProjectionMatrix;
}

void APortalPlayerController::PerformCameraCut()
{
	if (LocalPlayer != nullptr)
	{
		LocalPlayer->PerformCameraCut();
	}
}
