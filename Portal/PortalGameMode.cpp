// Copyright Epic Games, Inc. All Rights Reserved.

#include "PortalGameMode.h"
#include "PortalCharacter.h"
#include "UObject/ConstructorHelpers.h"

APortalGameMode::APortalGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
