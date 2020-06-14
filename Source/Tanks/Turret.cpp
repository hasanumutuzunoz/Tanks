// Fill out your copyright notice in the Description page of Project Settings.

#include "Turret.h"
#include "Tanks.h"
#include "PaperSpriteComponent.h"
#include "Tank.h"
#include "Missile.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components\InputComponent.h"


const FName ATurret::MuzzleSocketName(TEXT("Muzzle"));



// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = TurretDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("TurretDirection"));
	TurretSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TurretSprite"));
	TurretSprite->AttachToComponent(TurretDirection, FAttachmentTransformRules::KeepRelativeTransform);

	YawSpeed = 180.0f;
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
	check(GetParentComponent());
	Tank = Cast<ATank>(GetParentComponent()->GetOwner());
	check(Tank);

	//Being the child actor component implicitly doing this so we dont need this code. 
	//Be sure to aim only after the tank has turned and moved so that the turret doesn't lag one frame behind 
	/*
	if (ParentComponentActor.IsValid(())
	{
		AddTickPrerequisiteActor(ParentComponentActor.Get());
	}
	*/
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	check(TurretDirection);

	//UE_LOG(LogTemp, Warning, TEXT("AimInput X: (%f)"), CurrentInput.AimInput.X);
	//UE_LOG(LogTemp, Warning, TEXT("AimInput Y: (%f)"), TurretAim.AimInput.Y);

	if (Tank != nullptr)
	{
		if (APlayerController* PC = Cast<APlayerController>(Tank->GetController()))
		{

			/*
			//PC VERSION MOUSE AIM INPUT - Rotate the Turret with mouse DISABLED
			FVector2D AimLocation;
			if (PC->GetMousePosition(AimLocation.X, AimLocation.Y))
			{
				FVector2D TurretLocation = FVector2D::ZeroVector;
				UGameplayStatics::ProjectWorldToScreen(PC, TurretDirection->GetComponentLocation(), TurretLocation);
				float DesiredYaw;

				const FTankInput& CurrentInput = Tank->GetCurrentInput();
				if (UTankStatics::FindLookAtAngle2D(TurretLocation, CurrentInput.AimInput, DesiredYaw))
				//if (UTankStatics::FindLookAtAngle2D(TurretLocation, AimLocation, DesiredYaw))
				{
					FRotator CurrentRotation = TurretDirection->GetComponentRotation();
					float DeltaYaw = UTankStatics::FindDeltaAngleDegrees(CurrentRotation.Yaw, DesiredYaw);
					float MaxDeltaYawThisFrame = YawSpeed * DeltaTime;


					if (MaxDeltaYawThisFrame > FMath::Abs(DesiredYaw))
					{
						//We can get there in this frame, so just set it.
						CurrentRotation.Yaw = DesiredYaw;
					}
					else
					{
						CurrentRotation.Yaw += (FMath::Sign(DeltaYaw) * MaxDeltaYawThisFrame);

					}

					TurretDirection->SetWorldRotation(CurrentRotation);
					//UE_LOG(LogTemp, Warning, TEXT("CurrentRotation Yaw: (%f)"), CurrentRotation.Yaw);
					//UE_LOG(LogTemp, Warning, TEXT("Delta Yaw: (%f)"), DeltaYaw);
				}
			}
			*/


			//MOBILE VERSION AIM INPUT
			const FTankInput& CurrentInput = Tank->GetCurrentInput();
			FVector DesiredAimDirection = FVector(CurrentInput.AimInput.Y, CurrentInput.AimInput.X, 0.0f); //I switched the X and Y to fix  reverse rotation bug
			if (!DesiredAimDirection.IsNearlyZero())
			{

				//Rotate the tank!	We rotate the TankDirection component
				//Not the root component
				FRotator MovementAngle = DesiredAimDirection.Rotation();
				float DeltaYaw = UTankStatics::FindDeltaAngleDegrees(TurretDirection->GetComponentRotation().Yaw, MovementAngle.Yaw);
				//bool bReverse = false;
				if (DeltaYaw != 0.0f)
				{
					float AdjustedYaw = DeltaYaw;

					/*
					//Go backwards if we push 's' and/or other keys
					//Rotate 
					if (AdjustedYaw < -90.0f)
					{
						AdjustedYaw += 180.0f;
						bReverse = true;
					}
					else if (AdjustedYaw > 90.0f)
					{
						AdjustedYaw -= 180.0f;
						bReverse = true;
					}
					*/

					//Turn toward the desired angle. Stop if we can get there in one frame
					float MaxYawThisFrame = YawSpeed * DeltaTime;
					if (MaxYawThisFrame >= FMath::Abs(AdjustedYaw))
					{
						TurretDirection->SetWorldRotation(MovementAngle);


						/*
						if (bReverse)
						{
							//Move backwards
							FRotator FacingAngle = MovementAngle;
							FacingAngle.Yaw = MovementAngle.Yaw + 180.0f;
							TurretDirection->SetWorldRotation(FacingAngle);
						}
						else
						{
							// Finish, moving forward. Facing and movement are the same angle.
							TurretDirection->SetWorldRotation(MovementAngle);
						}*/
					}
					else
					{
						//Cant reach our desired angle in this frame, rotate part way (add MaxYawThisFrame to TankDirection everyframe) 
						TurretDirection->AddLocalRotation(FRotator(0.0f, FMath::Sign(AdjustedYaw) * MaxYawThisFrame, 0.0f));
					}
				}




				//Spawn the missile and move it
				//const FTankInput& CurrentInput = Tank->GetCurrentInput();
				if (CurrentInput.bFire1 && Projectile)
				{
					if (UWorld* World = GetWorld())
					{
						float CurrentTime = World->GetTimeSeconds();
						if (Fire1ReadyTime <= CurrentTime)
						{
							FVector Loc = TurretSprite->GetSocketLocation(MuzzleSocketName);
							FRotator Rot = TurretDirection->GetComponentRotation();

							if (AActor* NewProjectile = World->SpawnActor(Projectile))
							{
								UE_LOG(LogTemp, Warning, TEXT("Missile Launch"));
								NewProjectile->SetActorLocation(Loc);
								NewProjectile->SetActorRotation(Rot);
							}

							// Set the cooldown timer.
							Fire1ReadyTime = CurrentTime + Fire1Cooldown;
						}
					}
				}


			}
		}
	}
}

