// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalRoom.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "VRCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/EngineTypes.h"
#include "VRCharacter.h"

// Sets default values
APortalRoom::APortalRoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PRRoot"));
	SetRootComponent(PRRoot);

}

// Called when the game starts or when spawned
void APortalRoom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortalRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


