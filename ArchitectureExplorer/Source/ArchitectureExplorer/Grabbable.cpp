// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabbable.h"

// Sets default values
AGrabbable::AGrabbable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AGrabbable::BeginPlay()
{
	Super::BeginPlay();
	SetTickGroup(ETickingGroup::TG_PrePhysics);
}

// Called every frame
/*
void AGrabbable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *GetName());
}
*/
void AGrabbable::Grab()
{

}

