// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components\BoxComponent.h"
#include "Spawner.generated.h"

UCLASS()
class TANKS_API ASpawner : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ASpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorToSpawn;

	/** If anything matching this collision profile is present in the spawning volume, spawning will not be inhibited. */
	UPROPERTY(EditAnywhere)
	FName SpawnCollisionProfile;

	UPROPERTY()
	UBoxComponent* SpawnVolume;
	
};
