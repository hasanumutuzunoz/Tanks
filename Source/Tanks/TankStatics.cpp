// Fill out your copyright notice in the Description page of Project Settings.


#include "TankStatics.h" 
#include "TanksGameModeBase.h"
#include "Kismet\GameplayStatics.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"

float UTankStatics::FindDeltaAngleDegrees(float A1, float A2)
{
	//Find the difference
	float Delta = A2 - A1;

	
	if (Delta > 180.0f) //If change is larger than 180 degress
	{
		Delta = Delta - 360.0f; //Flip to negative equivalent
	}
	else if (Delta < -180.0f) //Else if change is smaller than 180 degress
	{
		Delta = Delta + 360.0f; //Flip to positive equivalent 
	}

	return Delta; // Return delta in [-180,180] range
}


bool UTankStatics::FindLookAtAngle2D(const FVector2D& Start, const FVector2D& Target, float& Angle)
{
	FVector2D Normal = (Target - Start).GetSafeNormal();
	if (!Normal.IsNearlyZero())
	{
		Angle = FMath::RadiansToDegrees(FMath::Atan2(Normal.Y, Normal.X));
		return true;
	}
	return false;
}

ATanksGameModeBase* UTankStatics::GetTanksGameMode(UObject* WorldContextObject)
{
	return Cast<ATanksGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
}

void UTankStatics::PutInZPlane(AActor* ActorToMove)
{
	if (ATanksGameModeBase* GM = UTankStatics::GetTanksGameMode(ActorToMove))
	{
		FVector Loc = ActorToMove->GetActorLocation();
		Loc.Z = GM->PlayInZPlane;
		ActorToMove->SetActorLocation(Loc);
	}
}

void UTankStatics::PlayFlipbook(UPaperFlipbookComponent* Component, UPaperFlipbook* NewFlipbook, bool bLooping, float PlayRate /*= 1.0f*/, bool bPlayFromStart /*= false*/)
{
	Component->SetFlipbook(NewFlipbook);
	Component->SetLooping(bLooping);
	Component->SetPlayRate(PlayRate);
	if (bPlayFromStart)
	{
		Component->PlayFromStart();
	}
	else
	{
		Component->Play();
	}
}
