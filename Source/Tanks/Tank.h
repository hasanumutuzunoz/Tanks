// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/ArrowComponent.h"	
//#include "Components\InputComponent.h"
#include "Camera\CameraComponent.h"
#include "DamageInterface.h"
//#include "PaperSpriteComponent.h"
#include "Tank.generated.h"

//This struct covers all possible tank input schemes
//What do inputs do can vary by tank, but the same inputs will always exist.
USTRUCT(BlueprintType)
struct FTankInput
{
	GENERATED_BODY()
		
public:

	void AimX(float AxisValue);
	void AimY(float AxisValue);

	//Sanitized aim input, ready to be used by game logic
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
	FVector2D AimInput;

	//Sanitized movement input, ready to be used by game logic
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
	FVector2D MovementInput;
	
	//1 byte bool, (its better to use than bool type)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
	uint32 bFire1 : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
	uint32 bFire2 : 1;

	
	void Sanitize();
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);
	void Fire1(bool bPressed);
	void Fire2(bool bPressed);

private:
	//Privae because its internal, raw data. Game code should never see this. 
	FVector2D RawMovementInput;

	//Privae because its internal, raw data. Game code should never see this. 
	FVector2D RawAimInput;
};


UCLASS()
class TANKS_API ATank : public APawn, public IDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATank();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable, Category = "Tank")
	const FTankInput& GetCurrentInput() { return TankInput; }
		
	//~ Begin IDamageInterface
	virtual void ReceiveDamage(int32 IncomingDamage, EDamageType DamageType) override;
	virtual int32 GetHealthRemaining() {return Health;}
	//~ End IDamageInterface

	UFUNCTION(BlueprintNativeEvent, Category = "Tank")
	void TankDie(EDamageType DamageType);

	//Our input structure
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
	FTankInput TankInput;


private:
	void AimX(float AxisValue);
	void AimY(float AxisValue);
	void MoveX(float AxisValue);  
	void MoveY(float AxisValue);
	void Fire1Pressed();
	void Fire1Released();
	void Fire2Pressed();
	void Fire2Released();
	
private:
	//Helpfull debugging tool - Which way is the tank facing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "tank", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* TankDirection;

	//Collision body for the tank
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "tank", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* TankBody;
	
	//Sprite for the tank body
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "tank", meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* TankSprite;

	//The actor used as the turret
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "tank", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* ChildTurret;

	//Our in-game camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "tank", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;
		
protected:
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gargant", meta = (ClampMin = "0.0"))
	//USoundBase* EnemyCrushed;


	

	//Maximum turn rate (degrees/second) of the tank
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
	float YawSpeed;

	// Acceleration for our tank, when player is trying to move or change directions.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
	float MoveAccel;

	//Maximum movement rate (units/second) of the tank
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
	float MoveSpeed;

	//Current health value. I can do different health values for diffent enemies
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
	int32 Health;

	// Collision profile for detecting (via overlaps) the things we can crush.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank")
	FName CrushCollisionProfile;
};