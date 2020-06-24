// Fill out your copyright notice in the Description page of Project Settings.


#include "Roach.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values
ARoach::ARoach()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RoachRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RoachRoot"));
	SetRootComponent(RoachRoot);

	RoachMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoachMesh"));
	RoachMesh->SetupAttachment(RoachRoot);

	ColQueryParams.AddIgnoredActor(this);

}

// Called when the game starts or when spawned
void ARoach::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ARoach::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (CheckFront())
	{
		LilTurn();
		UE_LOG(LogTemp, Warning, TEXT("Turning"));
	}
	else
	{
		Move();
		UE_LOG(LogTemp, Warning, TEXT("Moving"));
	}

}

void ARoach::Move()
{

	SetActorLocation(UKismetMathLibrary::VInterpTo_Constant(
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 10,
		GetWorld()->DeltaTimeSeconds,
		50.f
	));
}

bool ARoach::CheckFront()
{

	FVector AL = GetActorLocation();
	FVector AFV = GetActorForwardVector();
	bool Trace = GetWorld()->LineTraceSingleByChannel(
		HitResult, AL + AFV * 5, AL + AFV * 13, ECollisionChannel::ECC_WorldStatic, ColQueryParams);

	return true;

}

void ARoach::LilTurn()
{
	float DT = GetWorld()->DeltaTimeSeconds;
	FRotator AR = GetActorRotation();
	FRotator NR = AR;
	NR.Pitch += 10;

	FVector AL = GetActorLocation();
	FVector AFV = GetActorForwardVector();
	DrawDebugLine(GetWorld(), AL, AL + GetActorUpVector() * 16, FColor::Green, false, DT * 2);
	DrawDebugLine(GetWorld(), AL, AL + AFV * 16, FColor::Magenta, false, DT * 2);

	SetActorRotation(FMath::Lerp(AR, NR, DT));

	UE_LOG(LogTemp, Warning, TEXT("AR: %f | NR: %f"), AR.Pitch, NR.Pitch);

	
}
