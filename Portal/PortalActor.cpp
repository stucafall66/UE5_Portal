// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalActor.h"
#include "PortalCharacter.h"
#include "UTool.h"
#include "PortalPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "PortalPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APortalActor::APortalActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->Mobility = EComponentMobility::Static;

	PortalRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PortalRootComponent"));
	PortalRootComponent->SetupAttachment(GetRootComponent());
	PortalRootComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	PortalRootComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	PortalRootComponent->Mobility = EComponentMobility::Movable;

	// Create Mesh Plane and Bounding Box
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh"));
	PlaneMesh->SetupAttachment(RootComponent);
	PlaneMesh->SetRelativeRotation(FRotator(90.f, 0.f, 90.f));
	PlaneMesh->SetRelativeScale3D(FVector(3.f, 3.f, 3.f));

	PortalArea = CreateDefaultSubobject<UBoxComponent>(TEXT("PortalArea"));
	PortalArea->SetBoxExtent(FVector(50.f, 50.f, 32.f));
	PortalArea->SetupAttachment(PlaneMesh);
}

// Called when the game starts or when spawned
void APortalActor::BeginPlay()
{
	Super::BeginPlay();

	// Set ControllerOwner
	ControllerOwner = Cast<APortalPlayerController>(
		UGameplayStatics::GetPlayerController(this, 0));

	// Init SceneCapture
	SceneCapture = NewObject<USceneCaptureComponent2D>(
		this,
		USceneCaptureComponent2D::StaticClass(),
		*FString("PortalSceneCapture")
		);
	SceneCapture->AttachToComponent(
		GetRootComponent(),
		FAttachmentTransformRules::SnapToTargetIncludingScale
	);
	SceneCapture->RegisterComponent();
	
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bAlwaysPersistRenderingState = true;
	
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->TextureTarget = nullptr;
	SceneCapture->bEnableClipPlane = true;
	SceneCapture->bUseCustomProjectionMatrix = true;
	SceneCapture->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDRNoAlpha;
	SceneCapture->CompositeMode = ESceneCaptureCompositeMode::SCCM_Overwrite;
	
	// Setup Post-Process of SceneCapture
	FPostProcessSettings CaptureSettings;
	CaptureSettings.bOverride_AmbientOcclusionQuality = true;
	CaptureSettings.bOverride_MotionBlurAmount = true;
	CaptureSettings.bOverride_SceneFringeIntensity = true;
	CaptureSettings.bOverride_ScreenSpaceReflectionQuality = true;

	CaptureSettings.AmbientOcclusionQuality = 0.f;
	CaptureSettings.MotionBlurAmount = 0.f;
	CaptureSettings.SceneFringeIntensity = 0.f;
	CaptureSettings.ScreenSpaceReflectionQuality = 0.f;

	SceneCapture->PostProcessSettings = CaptureSettings;

	GeneratePortalTexture();
	SceneCapture->TextureTarget = PortalTexture;

	APortalActor* Target = GetLinkedPortal();
	if (Target != nullptr)
	{
		FVector TargetNormal = Target->PlaneMesh->GetUpVector();
		SceneCapture->ClipPlaneNormal = TargetNormal;
		SceneCapture->ClipPlaneBase = Target->GetActorLocation() +
			(SceneCapture->ClipPlaneNormal * -1.5f);
	}

}

void APortalActor::GeneratePortalTexture()
{
	int32 CurrentSizeX = 1920;
	int32 CurrentSizeY = 1080;

	FTransform BoundTransform;
	CurrentSizeX = FMath::FloorToInt32(PlaneMesh->CalcBounds(BoundTransform).BoxExtent.X);
	CurrentSizeY = FMath::FloorToInt32(PlaneMesh->CalcBounds(BoundTransform).BoxExtent.Y);
	float TextureScale = 1.f;
	if (CurrentSizeY < 2000.f)
	{
		TextureScale = 2000.f / CurrentSizeY;
	}
	if (TextureScale > 1.f)
	{
		CurrentSizeX = FMath::FloorToInt32(CurrentSizeX * TextureScale);
		CurrentSizeY = 2000;
	}
	UE_LOG(LogTemp, Warning, TEXT("PlaneMesh X is %d, Y is %d"), CurrentSizeX, CurrentSizeY);

	if (PortalTexture == nullptr)
	{
		PortalTexture = NewObject<UTextureRenderTarget2D>(
			this,
			UTextureRenderTarget2D::StaticClass(),
			*FString("PortalRenderTarget")
			);
		check(PortalTexture);

		PortalTexture->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
		PortalTexture->Filter = TextureFilter::TF_Bilinear;
		PortalTexture->SizeX = CurrentSizeX;
		PortalTexture->SizeY = CurrentSizeY;
		PortalTexture->ClearColor = FLinearColor::Black;
		PortalTexture->TargetGamma = 2.2f;
		PortalTexture->bNeedsTwoCopies = false;
		PortalTexture->AddressX = TextureAddress::TA_Wrap;
		PortalTexture->AddressY = TextureAddress::TA_Wrap;
		PortalTexture->bAutoGenerateMips = false;
		
		PortalTexture->UpdateResource();
	}
}

void APortalActor::UpdatePortal()
{
	if (ControllerOwner == nullptr || PortalCharacter == nullptr)
	{
		return;
	}
	FVector PortalLocation = GetActorLocation();
	FVector PlayerLocation = PortalCharacter->GetActorLocation();

	float Distance = FMath::Abs(FVector::Dist(PlayerLocation, PortalLocation));
	// UE_LOG(LogTemp, Warning, TEXT("Distance is %.2f"), Distance);
	if (Distance <= 1000)
	{
		if (bIsActive == false)
		{
			SetActive(true);
			SetRenderTarget(PortalTexture);
		}
	}
	else
	{
		if (bIsActive)
		{
			SetActive(false);
			ClearRenderTarget();
		}
	}
}

void APortalActor::UpdateCapture()
{
	if (SceneCapture == nullptr || PortalTexture == nullptr 
		|| PortalCharacter == nullptr || bIsActive == false)
	{
		return;
	}
	
	APortalActor* Target = GetLinkedPortal();
	// Place the SceneCapture to the linkedportal
	if (Target != nullptr)
	{
		UCameraComponent* PlayerCamera = PortalCharacter->GetFollowCamera();
		FVector NewLocation = UTool::ConvertCameraLocation(
			PlayerCamera->GetComponentLocation(),
			this,
			Target
		);
		SceneCapture->SetWorldLocation(NewLocation);

		FRotator NewRotation = UTool::ConvertRotationToActorSpace(
			PlayerCamera->GetComponentTransform().GetRotation().Rotator(),
			this,
			Target
		);
		
		SceneCapture->SetWorldRotation(NewRotation);
		SceneCapture->CustomProjectionMatrix = ControllerOwner->GetCameraProjectionMatrix();
		SceneCapture->CaptureScene();
	}
}

void APortalActor::RequestTeleportByPortal(AActor* TargetToTeleport)
{
	if (TargetToTeleport != nullptr)
	{
		TeleportActor(TargetToTeleport);
		// Force Update
		UpdatePortal();
		ForceTick();
		UpdateCapture();
	}
}

bool APortalActor::IsPointInsideBox(FVector Point, UBoxComponent* Box)
{
	if (Box != nullptr)
	{
		FVector Center = Box->GetComponentLocation();
		FVector Half = Box->GetScaledBoxExtent();
		FVector DirectionX = Box->GetForwardVector();
		FVector DirectionY = Box->GetRightVector();
		FVector DirectionZ = Box->GetUpVector();

		FVector Direction = Point - Center;
		bool IsInside = FMath::Abs(FVector::DotProduct(Direction, DirectionX)) <= Half.X
			&& FMath::Abs(FVector::DotProduct(Direction, DirectionY)) <= Half.Y
			&& FMath::Abs(FVector::DotProduct(Direction, DirectionZ)) <= Half.Z;
		return IsInside;
	}
	else
	{
		return false;
	}
}

bool APortalActor::IsActive()
{
	return bIsActive;
}

void APortalActor::SetActive(bool NewActive)
{
	bIsActive = NewActive;
}

void APortalActor::SetRenderTarget_Implementation(UTexture* RenderTexture)
{
}

void APortalActor::ClearRenderTarget_Implementation()
{
}

void APortalActor::ForceTick_Implementation()
{
}

APortalActor* APortalActor::GetLinkedPortal()
{
	return LinkedPortal;
}

void APortalActor::SetLinkedPortal(APortalActor* NewLinkedPortal)
{
	LinkedPortal = NewLinkedPortal;
}

bool APortalActor::IsPointInFrontOfPortal(FVector Point, FVector PortalLocation, FVector PortalNormal)
{
	FPlane PortalPlane = FPlane(PortalLocation, PortalNormal);
	float PortalPointDistance = PortalPlane.PlaneDot(Point);

	// If < 0, Point is behind plane
	return PortalPointDistance >= 0;
}

bool APortalActor::IsPointCrossingPortal(FVector Point, FVector PortalLocation, FVector PortalNormal)
{
	FVector IntersectionPoint;
	FPlane PortalPlane = FPlane(PortalLocation, PortalNormal);
	bool IsIntersect = FMath::SegmentPlaneIntersection(LastPosition, Point, PortalPlane, IntersectionPoint);

	LastPosition = Point;

	return IsIntersect;
}

void APortalActor::TeleportActor(AActor* ActorToTeleport)
{
	if (LinkedPortal == nullptr || ActorToTeleport == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LinkedPortal or ActorToTeleport is null"));
		return;
	}
	// Only Teleport when LastInFront is true
	if (LastInFront == false)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("TeleportActor"));

	if (ControllerOwner != nullptr)
	{
		ControllerOwner->PerformCameraCut();
	}

	FVector SavedVelocity = FVector::ZeroVector;
	SavedVelocity = ActorToTeleport->GetVelocity();

	// Apply new location/rotation for actor after
	FHitResult HitResult;
	FVector NewLocation = UTool::ConvertLocationToActorSpace(
		ActorToTeleport->GetActorLocation(),
		this,
		LinkedPortal);
	ActorToTeleport->SetActorLocation(NewLocation,
		false,
		&HitResult,
		ETeleportType::TeleportPhysics);

	FRotator NewRotation = UTool::ConvertRotationToActorSpace(
		ActorToTeleport->GetActorRotation(),
		this,
		LinkedPortal
	);
	ActorToTeleport->SetActorRotation(NewRotation);

	if (ActorToTeleport->IsA(APortalCharacter::StaticClass()))
	{
		APortalCharacter* Character = Cast<APortalCharacter>(ActorToTeleport);
		APortalPlayerController* PortalPlayerController = Character->GetController<APortalPlayerController>();
		if (PortalPlayerController != nullptr)
		{
			NewRotation = UTool::ConvertRotationToActorSpace(
				PortalPlayerController->GetControlRotation(),
				this,
				LinkedPortal
			);
			PortalPlayerController->SetControlRotation(NewRotation);
		}
		FVector NewVelocity = UTool::ConvertVelocityToActorSpace(SavedVelocity, this, LinkedPortal);
		Character->GetCharacterMovement()->Velocity = NewVelocity;
	}

	LastPosition = NewLocation;
	LinkedPortal->SetLastInFront(false);
	LastInFront = false;
}

void APortalActor::SetControllerOwner(APortalPlayerController* NewOwner)
{
	ControllerOwner = NewOwner;
	PortalCharacter = NewOwner->GetCharacter();
}

void APortalActor::SetLastInFront(bool IsInFront)
{
	LastInFront = IsInFront;
}

void APortalActor::Update()
{
	// Update this portal
	// Update Capture
	UpdatePortal();
	UpdateCapture();
}
