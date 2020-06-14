// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera\CameraShake.h"

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DamageInterface.h"
#include "Gargant.generated.h"

class ATank;

UCLASS()
class TANKS_API AGargant : public APawn, public IDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGargant();
	
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	

	//Set this enemies target. The base version of this function will handle updating TargetActor and TargetTank appropriately. 
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTarget(AActor* NewTarget);

	//Return the target actor. Returning NULL indicates no target 
	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTarget();

	//Return the target actor as a tank, if possible. Returning NULL indicates no target, or the target is not tank	
	UFUNCTION(BlueprintCallable, Category = "AI")
	ATank* GetTargetAsTank();
	


	//Gargants will call this on tick
	UFUNCTION(BlueprintNativeEvent, Category = "AI")
	void GargantAI(float DeltaSeconds);
	virtual void GargantAI_Implementation(float DeltaSeconds);


	//This function ask the gargant if it is in position to attack its current target. It does not acctually command gargant to attack. 
	UFUNCTION(BlueprintNativeEvent, Category = "AI")
	bool GargantAIShouldAttack();
	virtual bool GargantAIShouldAttack_Implementation();


	//This can be used to animate walking 
	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void ZombieWalk(float DeltaSeconds, FVector DistanceWalkedThisFrame);


	//This can be used to start animations 
	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void ZombieAttack(float DeltaSeconds);

	//This can be used to handle death from different sources
	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void ZombieDie(EDamageType DamageType);

	////////////////////////////////////////////////////////////////
	//////				 New Gargant Input					////////
	////////////////////////////////////////////////////////////////


	//Add rotation equal to the yaw (in degrees) provided
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
	virtual void AddRotationInput(float DeltaYawDegrees);

	//Get rotation input. Returns pre-clear value 
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
	virtual float GetRotationInput();

	//Get (and clear) rotation input. Return pre-clear value 
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
	virtual float ConsumeRotationInput();

	//Press the attack button. The pawn will know what to do with this. No arguments because its a bool
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
	virtual void AddAttackInput();

	//Get the status of the attack button
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
	virtual bool GetAttackInput();

	//Get (and clear) the status of the attack button  
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
	virtual bool ConsumeAttackInput();


	//~ Begin IDamageInterface
	virtual void ReceiveDamage(int32 IncomingDamage, EDamageType DamageType) override;
	virtual int32 GetHealthRemaining() override;
	//~ End IDamageInterface




private:
	//The actor we are targeting. Will be NULL if there is no target 
	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	AActor* TargetActor;

	//The actor we are targeting. Cast to Tank and cached. Will be NULL if there is no target
	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	ATank* TargetTank;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gargant", meta = (ClampMin = "0.0"))
	bool ZombieDead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gargant", meta = (ClampMin = "0.0"))
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gargant", meta = (ClampMin = "0.0"))
	USoundBase* TankReceiveDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gargant", meta = (ClampMin = "0.0"))
	USoundBase* GargantDie;


	//Camera effect, if any, to play when the player is hit by enemy
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake> HitShake;
	 
	//Current health value. I can do different health values for diffent enemies
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gargant", meta = (ClampMin = "0.0"))
	int32 Health;		

	//Sight distance (when no target is present)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gargant", meta = (ClampMin = "0.0"))
	float SightDistance;

	//Sight half-angle in degrees (when no trget is present)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gargant", meta = (ClampMin = "0.0"))
	float SightAngle;

	//Max yaw speed per second 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gargant", meta = (ClampMin = "0.0"))
	float YawSpeed;

	//Speed when wandering around 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gargant", meta = (ClampMin = "0.0"))
	float WalkSpeed;

	//Speed when rushing toward target
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gargant", meta = (ClampMin = "0.0"))
	float RunSpeed;

	//Attack Distance
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gargant", meta = (ClampMin = "0.0"))
	float AttackDistance;

	//Half-angle in degrees for our attack 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gargant", meta = (ClampMin = "0.0"))
	float AttackAngle;

	//Minimum time between attacks
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gargant", meta = (ClampMin = "0.0"))
	float AttackCooldown;

	//Game time in seconds, when the enemy will ebe allowed to attack again
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadWrite, Category = "Gargant")
	float AttackAvailableTime;

private:
	// Current rotation input
	float YawInput;

	// Current attack button status 
	uint32 bAttackInput : 1;

};
