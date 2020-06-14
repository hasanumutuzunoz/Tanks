// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TankStatics.generated.h"


UCLASS()
class TANKS_API UTankStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//Calculate angle difference in the range [-180, 180]
	UFUNCTION(BlueprintCallable, Category = "Math")
	static float FindDeltaAngleDegrees(float A1, float A2);
	
	//Find the angle between two 2D points
	UFUNCTION(BlueprintPure, Category = "Math|Rotator", meta = (Keywords = "rotation rotate"))
	static bool FindLookAtAngle2D(const FVector2D& start, const FVector2D& Target, float& angle);

	/** Get the current game mode, pre-cast to ATanksGameMode for convenience. */
	UFUNCTION(BlueprintCallable, Category = "Tanks")
	static class ATanksGameModeBase* GetTanksGameMode(UObject* WorldContextObject);

	/** Snap this actor to the Z plane that we're using for this level. */
	UFUNCTION(BlueprintCallable, Category = "Tanks")
	static void PutInZPlane(AActor* ActorToMove);

	/** All-in-one convenience function to play a flipbook. */
	UFUNCTION(BlueprintCallable, Category = "Animation")
	static void PlayFlipbook(class UPaperFlipbookComponent* Component, class UPaperFlipbook* NewFlipbook, bool bLooping, float PlayRate = 1.0f, bool bPlayFromStart = false);

};
