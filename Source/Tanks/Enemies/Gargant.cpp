// Fill out your copyright notice in the Description page of Project Settings.

#include "Gargant.h"
#include "Tanks.h"
#include "Tank.h"
#include "GameFramework\PlayerController.h"

// Sets default values
AGargant::AGargant()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	Health = 15;
	SightDistance = 500.0f;
	SightAngle = 60.0f;
	YawSpeed = 90.0f;
	WalkSpeed = 25.0f;
	RunSpeed = 45.0f;
	AttackDistance = 100.0f;
	AttackAngle = 30.0f;
	AttackCooldown = 1.0f;
}

// Called when the game starts or when spawned
void AGargant::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGargant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GargantAI(DeltaTime);

	//Make sure to consume all input on each frame 
	ConsumeMovementInputVector();
	ConsumeRotationInput();
	ConsumeAttackInput();
}

// Called to bind functionality to input
void AGargant::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


//GARGANT MELEE ATTACK IF NEAR TANK
void AGargant::GargantAI_Implementation(float DeltaSeconds) 
{
	// The gargant always moves unless attacking. If moving, it moves between WalkSpeed and RunSpeed.
	FVector DesiredMovement = GetAttackInput() ? FVector::ZeroVector : (FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(WalkSpeed, RunSpeed), GetPendingMovementInputVector().X)) * DeltaSeconds * GetActorForwardVector();
	FVector OriginalLocation = GetActorLocation();
	FVector DesiredLoc = OriginalLocation + DesiredMovement;
	float MaxYawThisFrame = YawSpeed * DeltaSeconds;
	FRotator DesiredRot = GetActorRotation() + FRotator(0.0f, FMath::Clamp(GetRotationInput(), -MaxYawThisFrame, MaxYawThisFrame), 0.0f);

	if (!ZombieDead)
	{
		SetActorLocationAndRotation(DesiredLoc, DesiredRot.Quaternion(), true);
	}
	FVector DistanceWalked = GetActorLocation() - OriginalLocation;


	if (!DistanceWalked.IsNearlyZero() && !ZombieDead)
	{
		//Set the speed of the walking animation 
		ZombieWalk(DeltaSeconds, DistanceWalked);
	}

	//See if we have a target and deal with it if we do. Find a target if we dont 
	if (AActor* Target = GetTarget())
	{
		//We do have a target. Check if we are attacking 
		FVector OurLocation = GetActorLocation();
		FVector DirectionToTarget = (Target->GetActorLocation() - OurLocation.GetSafeNormal2D());
		float DotToTarget = FVector::DotProduct(DirectionToTarget, GetActorForwardVector());

		//Check if we should attack based on button status and attack cooldown
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (GetAttackInput() && (AttackAvailableTime <= CurrentTime) && !ZombieDead)
		{
			AttackAvailableTime = CurrentTime + AttackCooldown;
			ZombieAttack(DeltaSeconds);

			//If it's in angle 
			if (DotToTarget >= FMath::Cos(FMath::DegreesToRadians(AttackAngle)))
			{
				float DistSqXY = FVector::DistSquaredXY(Target->GetActorLocation(), OurLocation);

				//If it's in distance 
				if (DistSqXY <= (AttackDistance * AttackDistance))
				{
					if (ATank* TankTarget = GetTargetAsTank())
					{
						TankTarget->ReceiveDamage(AttackDamage, EDamageType::GargantSlap);
						if (APlayerController* PC = Cast<APlayerController>(TankTarget->GetController()))
						{
							UE_LOG(LogTemp, Warning, TEXT("Attack!!"));
							PC->ClientPlayCameraShake(HitShake, 10.0f);
							PC->ClientPlaySound(TankReceiveDamage, 1.0f, 1.0f);
						}
					}
					else
					{
						SetTarget(nullptr);
					}
				}
			}
		}
	}
	else
	{
		// Look for a target. We might not do this every single frame, but for now it's OK.
		// TODO: Make this use a list of registered targets so we can handle multiplayer or add decoys.
		Target = UGameplayStatics::GetPlayerPawn(this, 0);
		float DistSqXY = FVector::DistSquaredXY(Target->GetActorLocation(), GetActorLocation());
		//If the tank is in distance
		if (DistSqXY <= (SightDistance * SightDistance))
		{
			FVector DirectionToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
			//If the tank is in angle 
			if (FVector::DotProduct(DirectionToTarget, GetActorForwardVector()) >= FMath::Cos(FMath::DegreesToRadians(SightAngle)))
			{
				SetTarget(Target);
			}
		}
	}
}

//SHOULD GARGANT ATTACK BOOLEAN
bool AGargant::GargantAIShouldAttack_Implementation()
{
	if (AActor* Target = GetTarget())
	{
		//Attack our target if we are in range (distance and angle). 
		FVector OurLocation = GetActorLocation();
		FVector DirectionToTarget = (Target->GetActorLocation() - OurLocation.GetSafeNormal2D());
		float DotToTarget = FVector::DotProduct(DirectionToTarget, GetActorForwardVector());

		//If it's in angle 
		if (DotToTarget >= FMath::Cos(FMath::DegreesToRadians(AttackAngle)))
		{
			float DistSqXY = FVector::DistSquaredXY(Target->GetActorLocation(), OurLocation);

			//If it's in distance 
			if (DistSqXY <= (AttackDistance * AttackDistance))
			{
				//Note that attacking cooldown isn't checked. We dont want this kind of zombie to move 
				return true; //attack
			}
		}
	}
	return false;
}


void AGargant::SetTarget(AActor* NewTarget)
{
	TargetActor = NewTarget;
	TargetTank = Cast<ATank>(NewTarget);
}

AActor* AGargant::GetTarget()
{
	return TargetActor;
}

ATank* AGargant::GetTargetAsTank()
{
	return TargetTank;
}


void AGargant::AddRotationInput(float DeltaYawDegrees)
{
	YawInput += DeltaYawDegrees;
}


float AGargant::GetRotationInput()
{
	return YawInput;
}

float AGargant::ConsumeRotationInput()
{
	float RetVal = YawInput;
	YawInput = 0.0f;
	return RetVal;
}

void AGargant::AddAttackInput()
{
	bAttackInput = true;
}

bool AGargant::GetAttackInput()
{
	return bAttackInput;
}

bool AGargant::ConsumeAttackInput()
{
	bool bRetVal = bAttackInput;
	bAttackInput = false;
	return bRetVal;
}

void AGargant::ReceiveDamage(int32 IncomingDamage, EDamageType DamageType)
{
	if (IncomingDamage >= Health)
	{
		if (Health >= 0)
		{
			Health = -1;
			ZombieDead = true;
			ZombieDie(DamageType);
		}
		return;
	}
	Health -= IncomingDamage;
}

int32 AGargant::GetHealthRemaining()
{
	return Health;
}




