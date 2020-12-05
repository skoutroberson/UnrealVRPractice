// Fill out your copyright notice in the Description page of Project Settings.


#include "Stage3.h"

AStage3::AStage3()
{
	AStage::FlagCount = 3;
	AStage::Flags.Init(false, FlagCount);
}

AStage3::~AStage3()
{
	
}

void AStage3::BeginPlay()
{
	Super::BeginPlay();
}

void AStage3::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	RemoveTriggerDelegates();
}

void AStage3::AddTriggerDelegates()
{

}

void AStage3::RemoveTriggerDelegates()
{

}
