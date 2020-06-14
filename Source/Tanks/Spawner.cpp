// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"
#include "Kismet\KismetMathLibrary.h"
#include "TankStatics.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 2.0f;

	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	FVector BoxExtent; //= SpawnVolume->GetUnscaledBoxExtent();
	BoxExtent.Z = 100.0f;
	BoxExtent.X = 600.0f;
	BoxExtent.Y = 600.0f;
	SpawnVolume->SetBoxExtent(BoxExtent, false);
	if (RootComponent)
	{
		SpawnVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	else
	{
		RootComponent = SpawnVolume;
	}
	SpawnCollisionProfile = TEXT("SpawnerBlock");
}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();
	
	UTankStatics::PutInZPlane(this);
	
}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActorToSpawn)
	{
		if (UWorld* World = GetWorld())
		{
			FCollisionShape CollisionShape;
			CollisionShape.SetBox(SpawnVolume->GetUnscaledBoxExtent());
			//If no collision with profile in SpawnVolume
			if (!World->OverlapAnyTestByProfile(SpawnVolume->GetComponentLocation(), SpawnVolume->GetComponentRotation().Quaternion(), SpawnCollisionProfile, CollisionShape))
			{
				//Spawn the ActorToSpawn
				if (AActor* SpawnedActor = World->SpawnActor(ActorToSpawn->GetDefaultObject()->GetClass()))
				{
					FVector RandomEnemyLoc = UKismetMathLibrary::RandomPointInBoundingBox(SpawnVolume->GetComponentLocation(), SpawnVolume->GetUnscaledBoxExtent());
					SpawnedActor->SetActorLocation(RandomEnemyLoc);
					UTankStatics::PutInZPlane(SpawnedActor);
				}
			}
		}
	}
}

