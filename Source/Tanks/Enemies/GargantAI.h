// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GargantAI.generated.h"

class AGargant; 
/**
 * 
 */
UCLASS()
class TANKS_API AGargantAI : public AAIController
{
	GENERATED_BODY()
public:
    
    virtual void Tick (float DeltaTime) override;

    virtual void OnPossess(APawn* InPawn) override;

    virtual void OnUnPossess() override;


protected:
    //Our pawn pre-casts to a gargant. Will be NULL if the pawn is not a gargant.
    UPROPERTY(BlueprintReadOnly, Category = "Gargant")
    AGargant* PawnAsGargant;

};
