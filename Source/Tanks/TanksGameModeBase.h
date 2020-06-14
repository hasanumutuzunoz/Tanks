// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TanksGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TANKS_API ATanksGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATanksGameModeBase();

	// Every Zombie game actor should snap to this Z value during BeginPlay. E.g. zombies, tanks, missiles. Default is 0.
	UPROPERTY(EditAnywhere)
	float PlayInZPlane;
};
