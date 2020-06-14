// Fill out your copyright notice in the Description page of Project Settings.


#include "Missile.h"
#include "Engine/World.h"
#include "DamageInterface.h"
#include "Tank.h"
#include "Engine/Public/TimerManager.h"

// Sets default values
AMissile::AMissile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Speed = 200.0f;
	Radius = 20.0f;
	DirectDamage = 5;
	
}

// Called when the game starts or when spawned
void AMissile::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(ExplodeTimerHandle, this, &AMissile::Explode, 1.0f);		
	//UE_LOG(LogTemp, Warning, TEXT("Missile!"));
}

// Called every frame
void AMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector Loc = GetActorLocation();
	FVector DesiredEndLoc = Loc + (DeltaTime * Speed) * GetTransform().GetUnitAxis(EAxis::Y);
	FHitResult OutHit;
	FCollisionShape CollisionShape;
	
	if (Radius > 0.0f)//We can have grenade or other types of weapon which doesnt need to collide, thats why we used this if radius > 0
	{
		if (UWorld* World = GetWorld())
		{
			CollisionShape.SetCapsule(Radius, 200.0f);
			//If hit - then apply damage and explode
			if (World->SweepSingleByProfile(OutHit, Loc, DesiredEndLoc, FQuat::Identity, MovementCollisionProfile, CollisionShape))
			{
				SetActorLocation(OutHit.Location);
				if (IDamageInterface* DamageActor = Cast<IDamageInterface>(OutHit.Actor.Get()))
				{		
					if (!MissileHit)
					{
						DamageActor->ReceiveDamage(DirectDamage, EDamageType::HitWithMissile);
						UE_LOG(LogTemp, Warning, TEXT("Gargant Health: %f"), DamageActor->GetHealthRemaining());
						Explode();
					}
				}
				MissileHit = true;
			}
			else
			{
				SetActorLocation(DesiredEndLoc);
			}
		}
	}
}

void AMissile::Explode()
{
	GetWorldTimerManager().ClearTimer(ExplodeTimerHandle);
	SetActorEnableCollision(false);
	OnExplode(); 
}

void AMissile::OnExplode_Implementation()
{
	Destroy();
}







