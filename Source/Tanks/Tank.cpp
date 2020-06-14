// Fill out your copyright notice in the Description page of Project Settings.

#include "Tank.h"
#include "Tanks.h"
#include "PaperSpriteComponent.h"
#include "Enemies\Gargant.h"
#include "GameFramework\SpringArmComponent.h"
#include "Components\InputComponent.h"
//#include "GameFramework\Actor.h"

void FTankInput::AimX(float AxisValue)
{
	RawAimInput.X += AxisValue;
}

void FTankInput::AimY(float AxisValue)
{
	RawAimInput.Y += AxisValue;
}

void FTankInput::Sanitize()
{
	MovementInput = RawMovementInput.ClampAxes(-1.0f, 1.0f);
	MovementInput = MovementInput.GetSafeNormal();
	RawMovementInput.Set(0.0f, 0.0f);

	AimInput = RawAimInput.ClampAxes(-1.0f, 1.0f);
	AimInput = AimInput.GetSafeNormal();
	RawAimInput.Set(0.0f, 0.0f);
}

void FTankInput::MoveX(float AxisValue)
{
	RawMovementInput.X += AxisValue;
}

void FTankInput::MoveY(float AxisValue)
{
	RawMovementInput.Y += AxisValue;
}

void FTankInput::Fire1(bool bPressed)
{
	bFire1 = bPressed;
	//UE_LOG(LogTemp, Warning, TEXT("bPressed: %f"), bPressed);
}

void FTankInput::Fire2(bool bPressed)
{
	bFire2 = bPressed;
}



// Sets default values
ATank::ATank()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TankBase"));
	}
			
	TankDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("TankDirection"));
	TankDirection->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	TankBody = CreateDefaultSubobject<UBoxComponent>(TEXT("TankBody"));
	TankBody->AttachToComponent(TankDirection, FAttachmentTransformRules::KeepRelativeTransform);
	TankBody->SetBoxExtent(FVector(40.0f, 40.0f, 100.0f));
	TankSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Tankprite"));
	TankSprite->AttachToComponent(TankDirection, FAttachmentTransformRules::KeepRelativeTransform);
	
	USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->CameraLagSpeed = 2.0f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SpringArm->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	CameraComponent->OrthoWidth = 1024.0f;
	CameraComponent->AspectRatio = 3.0f / 4.0f;
	CameraComponent->AttachToComponent(SpringArm, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
	CameraComponent->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));

	ChildTurret = CreateDefaultSubobject<UChildActorComponent>(TEXT("Turret"));
	ChildTurret->AttachToComponent(TankDirection, FAttachmentTransformRules::KeepRelativeTransform);

	MoveSpeed = 100.0f;
	YawSpeed = 180.0f;
	MoveAccel = 200.0f;

	CrushCollisionProfile = TEXT("TankCrush");
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TankInput.Sanitize();

	//UE_LOG(LogTemp, Warning, TEXT("AimInput X: (%f)"), TankInput.AimInput.X);
	//UE_LOG(LogTemp, Warning, TEXT("AimInput Y: (%f)"), TankInput.AimInput.Y);
	//UE_LOG(LogTemp, Warning, TEXT("RawMovementInput X: (%f)"), TankInput.RawMovementInput.X);
	//UE_LOG(LogTemp, Warning, TEXT("RawMovementInput Y: (%f)"), TankInput.RawMovementInput.Y);
	//UE_LOG(LogTemp, Warning, TEXT("Movement: (%f, %f)"), TankInput.MovementInput.X, TankInput.MovementInput.Y);

	if (GetHealthRemaining() >= 0)
	{
		//Move the tank! 
		FVector DesiredMovementDirection = FVector(TankInput.MovementInput.X, TankInput.MovementInput.Y, 0.0f);
		if (!DesiredMovementDirection.IsNearlyZero())
		{
			//Rotate the tank!	We rotate the TankDirection component
			//Not the root component
			FRotator MovementAngle = DesiredMovementDirection.Rotation();
			float DeltaYaw = UTankStatics::FindDeltaAngleDegrees(TankDirection->GetComponentRotation().Yaw, MovementAngle.Yaw);
			bool bReverse = false;
			if (DeltaYaw != 0.0f)
			{
				float AdjustedYaw = DeltaYaw;

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

				//Turn toward the desired angle. Stop if we can get there in one frame
				float MaxYawThisFrame = YawSpeed * DeltaTime;
				if (MaxYawThisFrame >= FMath::Abs(AdjustedYaw))
				{
					if (bReverse)
					{
						//Move backwards
						FRotator FacingAngle = MovementAngle;
						FacingAngle.Yaw = MovementAngle.Yaw + 180.0f;
						TankDirection->SetWorldRotation(FacingAngle);
					}
					else
					{
						// Finish, moving forward. Facing and movement are the same angle.
						TankDirection->SetWorldRotation(MovementAngle);
					}
				}
				else
				{
					//Cant reach our desired angle in this frame, rotate part way (add MaxYawThisFrame to TankDirection everyframe) 
					TankDirection->AddLocalRotation(FRotator(0.0f, FMath::Sign(AdjustedYaw) * MaxYawThisFrame, 0.0f));
				}
			}
			
			//Move the Tank
			{
				FVector MovementDirection = TankDirection->GetForwardVector() * (bReverse ? -1.0f : 1.0f);
				FVector StartPos = GetActorLocation();
				FVector Pos = StartPos;
				Pos.X += MovementDirection.X * MoveSpeed * DeltaTime;
				Pos.Y += MovementDirection.Y * MoveSpeed * DeltaTime;
				
				
				//Check if tank collides with anything, if so, apply damage to enemies
				if (UWorld* World = GetWorld())
				{
					TArray<FHitResult> HitResults;
					FVector BoxSize = TankBody->GetScaledBoxExtent();
					FCollisionShape CollisionShape;
					CollisionShape.SetBox(BoxSize);
					World->SweepMultiByProfile(HitResults, StartPos, Pos, TankBody->GetComponentRotation().Quaternion(), CrushCollisionProfile, CollisionShape);
					for (const FHitResult& HitResult : HitResults)
					{
						if (IDamageInterface* DamageTarget = Cast<IDamageInterface>(HitResult.Actor.Get()))
						{
							// Getting crushed by a tank is DEATH!
							int32 TargetHealth = DamageTarget->GetHealthRemaining();
							if (TargetHealth >= 0)
							{
								DamageTarget->ReceiveDamage(TargetHealth, EDamageType::Crushed);
								/*
								if (APlayerController* PC = Cast<APlayerController>(GetController()))
								{
									PC->ClientPlaySound(EnemyCrushed, 1.0f, 1.0f);
								}
								*/
							}
						}
					}
				}
				SetActorLocation(Pos);
			}
		}
	}
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveX", this, &ATank::MoveX);
	PlayerInputComponent->BindAxis("MoveY", this, &ATank::MoveY);
	PlayerInputComponent->BindAxis("AimX", this, &ATank::AimX);
	PlayerInputComponent->BindAxis("AimY", this, &ATank::AimY);
	PlayerInputComponent->BindAction("Fire1", EInputEvent::IE_Pressed, this, &ATank::Fire1Pressed);
	PlayerInputComponent->BindAction("Fire1", EInputEvent::IE_Released, this, &ATank::Fire1Released);
	PlayerInputComponent->BindAction("Fire2", EInputEvent::IE_Pressed, this, &ATank::Fire2Pressed);
	PlayerInputComponent->BindAction("Fire2", EInputEvent::IE_Released, this, &ATank::Fire2Released);
}

void ATank::AimX(float AxisValue)
{
	TankInput.AimX(AxisValue);
}

void ATank::AimY(float AxisValue)
{
	TankInput.AimY(AxisValue);
}

void ATank::MoveX(float AxisValue)
{
	TankInput.MoveX(AxisValue);
}

void ATank::MoveY(float AxisValue)
{
	TankInput.MoveY(AxisValue);
}

void ATank::Fire1Pressed()
{
	TankInput.Fire1(true);	
}

void ATank::Fire1Released()
{
	TankInput.Fire1(false);
}

void ATank::Fire2Pressed()
{
	TankInput.Fire2(true);
}

void ATank::Fire2Released()
{
	TankInput.Fire1(false);
}


void ATank::ReceiveDamage(int32 IncomingDamage, EDamageType DamageType)
{
	if (IncomingDamage >= Health)
	{
		if (Health >= 0)
		{
			Health = -1;
			TankDie(DamageType);
		}
		return;		
	}
	Health -= IncomingDamage;
}

void ATank::TankDie_Implementation(EDamageType DamageType)
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// TODO: Manage extra lives?
		PC->RestartLevel();
	}
}

