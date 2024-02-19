// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperSprintShowGameMode.h"
#include "SuperSprintShowCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASuperSprintShowGameMode::ASuperSprintShowGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

//// test