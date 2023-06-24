// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalActor.generated.h"

class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class APortalPlayerController;
class APortalCharacter;

UCLASS()
class PORTAL_API APortalActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalActor();

	UFUNCTION(BlueprintPure)
	bool IsActive();

	UFUNCTION(BlueprintCallable)
	void SetActive(bool NewActive);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetRenderTarget(UTexture* RenderTexture);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ClearRenderTarget();

	UFUNCTION(BlueprintNativeEvent)
	void ForceTick();

	UFUNCTION(BlueprintPure)
	APortalActor* GetLinkedPortal();

	UFUNCTION(BlueprintCallable)
	void SetLinkedPortal(APortalActor* NewLinkedPortal);

	UFUNCTION(BlueprintCallable)
	bool IsPointInFrontOfPortal(FVector Point, FVector PortalLocation, FVector PortalNormal);

	UFUNCTION(BlueprintCallable)
	bool IsPointCrossingPortal(FVector Point, FVector PortalLocation, FVector PortalNormal);

	UFUNCTION(BlueprintCallable)
	void TeleportActor(AActor* ActorToTeleport);

	UFUNCTION(BlueprintCallable)
	void Update();

	// Save a reference to the PlayerController
	void SetControllerOwner(APortalPlayerController* NewOwner);

	UFUNCTION(BlueprintCallable)
	void SetLastInFront(bool IsInFront);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PlaneMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* PortalArea;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(BlueprintReadOnly)
	USceneComponent* PortalRootComponent;

private:
	void GeneratePortalTexture();
	void UpdatePortal();
	void UpdateCapture();

	UFUNCTION(BlueprintCallable)
	void RequestTeleportByPortal(AActor* TargetToTeleport);

	UFUNCTION(BlueprintPure)
	bool IsPointInsideBox(FVector Point, class UBoxComponent* Box);

private:	
	UPROPERTY()
	USceneCaptureComponent2D* SceneCapture;

	UPROPERTY(Transient)
	UTextureRenderTarget2D* PortalTexture;

	UPROPERTY()
	APortalPlayerController* ControllerOwner;

	bool bIsActive{false};
	FVector LastPosition;
	bool LastInFront;

	UPROPERTY()
	APortalCharacter* PortalCharacter;

	UPROPERTY()
	APortalActor* LinkedPortal;
};
