// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Stage.h"
#include <vector>
#include "BoxTrigger.h"
#include "Engine/TriggerBox.h"
#include "Engine/World.h"
#include "Door.h"
#include "Stage1.generated.h"

using namespace std;

/**
 * 
 */
UCLASS()
class ARCHITECTUREEXPLORER_API AStage1 : public AStage
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AStage1();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	ADoor * TheDoor = nullptr;
};
