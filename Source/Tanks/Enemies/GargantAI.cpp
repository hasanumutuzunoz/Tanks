// Fill out your copyright notice in the Description page of Project Settings.

#include "GargantAI.h" 
#include "Gargant.h"

void AGargantAI::Tick(float DeltaTime)
{
	if (PawnAsGargant)
	{	
		if (AActor* Target = PawnAsGargant->GetTarget())
		{
			//We do have a target. Shamble toward it and attempt violance
			FVector DirectionToTarget = (Target->GetActorLocation() - PawnAsGargant->GetActorLocation()).GetSafeNormal2D();
			float DotToTarget = FVector::DotProduct(DirectionToTarget, PawnAsGargant->GetActorForwardVector());
			float SidewaysDotToTarget = FVector::DotProduct(DirectionToTarget, PawnAsGargant->GetActorRightVector());
			float DeltaYawDesired = FMath::Atan2(SidewaysDotToTarget, DotToTarget);
			
			if (PawnAsGargant->GargantAIShouldAttack())
			{
				PawnAsGargant->AddAttackInput();
			}

			else
			{
				//Move faster when facing toward the target so that we turn more accurately/don't orbit.
				PawnAsGargant->AddMovementInput(FVector(1.0f, 0.0f, 0.0f), FMath::GetMappedRangeValueClamped(FVector2D(-0.707f, 0.707f), FVector2D(0.0f, 1.0f), DotToTarget));
				// Attempt the entire turn in one frame. The Gargant itself will cap this, we're only expressing our desired turn amount here.
				PawnAsGargant->AddRotationInput(DeltaYawDesired);
			}
		}
	}
}

void AGargantAI::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	PawnAsGargant = Cast<AGargant>(GetPawn());
}

void AGargantAI::OnUnPossess()
{
	Super::OnUnPossess();
	PawnAsGargant = nullptr;
}