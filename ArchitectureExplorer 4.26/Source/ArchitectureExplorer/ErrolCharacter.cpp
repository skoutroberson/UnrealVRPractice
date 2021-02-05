// Fill out your copyright notice in the Description page of Project Settings.


#include "ErrolCharacter.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "AIController.h"
#include "AITypes.h"
#include "ErrolController.h"
#include "Perception/AIPerceptionComponent.h"
#include "VRCharacter.h"
#include "HandController.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AErrolCharacter::AErrolCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	State = ErrolState::STATE_IDLE;
}

// Called when the game starts or when spawned
void AErrolCharacter::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();

	ErrolEye = Cast<USceneComponent>(GetComponentsByTag(USceneComponent::StaticClass(), FName("ET"))[0]);

	ErrolController = Cast<AErrolController>(GetController());
	ErrolController->InitializeLookAroundTimer();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), Waypoints);

	// I have to call this a bit after the game starts so the player and hand controllers are spawned in
	GetWorld()->GetTimerManager().SetTimer(SetUpCanSeeHandle, this, &AErrolCharacter::InitializeCanSeeVariables, SeeTimerRate, false, 0.2f);
	EnterPatrolState();
}

// Called every frame
void AErrolCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (State)
	{
	case ErrolState::STATE_IDLE:
		//UE_LOG(LogTemp, Warning, TEXT("IDLE"));
		break;
	case ErrolState::STATE_PATROL:
		//UE_LOG(LogTemp, Warning, TEXT("PATROL"));
		UpdateSpeedBasedOnRotation();
		break;
	case ErrolState::STATE_CHASE:
		//UE_LOG(LogTemp, Warning, TEXT("CHASE"));
		break;
	}

}

// Called to bind functionality to input
void AErrolCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AErrolCharacter::Patrol()
{
	// Move to random places on Nav Mesh with distance from player based on a variable.
}

ATargetPoint * AErrolCharacter::GetRandomWaypoint()
{
	auto index = FMath::RandRange(0, Waypoints.Num() - 1);
	return Cast<ATargetPoint>(Waypoints[index]);
}

void AErrolCharacter::GoToRandomWaypoint()
{
	ErrolController->MoveToActor(GetRandomWaypoint(), -1.f, true, true, true);
	//AAIController * AICon = Cast<AAIController>(GetController());
	//AICon->MoveToActor(GetRandomWaypoint(), -1.f, true, true, true);
}

void AErrolCharacter::EnterIdleState()
{
	State = ErrolState::STATE_IDLE;
}

void AErrolCharacter::EnterPatrolState()
{
	State = ErrolState::STATE_PATROL;
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	GoToRandomWaypoint();
}

void AErrolCharacter::EnterChaseState()
{
	State = ErrolState::STATE_CHASE;
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	ErrolController->MoveToActor(Player);
}

void AErrolCharacter::ExitIdleState()
{
}

void AErrolCharacter::ExitPatrolState()
{
	State = ErrolState::STATE_IDLE;
	ErrolController->StopMovement();
}

void AErrolCharacter::ExitChaseState()
{
}

void AErrolCharacter::InitializeCanSeeVariables()
{
	UE_LOG(LogTemp, Warning, TEXT("Initalize can see!"));
	GetWorld()->GetTimerManager().ClearTimer(SetUpCanSeeHandle);
	Player = Cast<AActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AVRCharacter::StaticClass()));
	AVRCharacter * TempC = Cast<AVRCharacter>(Player);
	LHandController = Cast<AActor>(TempC->LeftController);
	RHandController = Cast<AActor>(TempC->RightController);
	UActorComponent * TempAC = Player->GetComponentByClass(UCameraComponent::StaticClass());
	PlayerCamera = Cast<UCameraComponent>(TempAC);
	InitializePerceptionTimers();
}

void AErrolCharacter::InitializePerceptionTimers()
{
	GetWorld()->GetTimerManager().SetTimer(ChaseTimerHandle, this, &AErrolCharacter::SetSeeGauge, SeeTimerRate, true);
}

void AErrolCharacter::SetSeeGauge()
{	
	//UE_LOG(LogTemp, Warning, TEXT("See: %d"), CanSeeHitCounter);
	FHitResult Outhit;
	FCollisionQueryParams EyeParams;
	EyeParams.AddIgnoredActor(this);
	EyeParams.AddIgnoredActor(Player);
	FVector EyeLocation = ErrolEye->GetComponentLocation();
	FVector CameraLocation = PlayerCamera->GetComponentLocation();
	FVector LHCLocation = LHandController->GetActorLocation();
	FVector RHCLocation = RHandController->GetActorLocation();

	bool Trace1 = World->LineTraceSingleByChannel(Outhit, EyeLocation, CameraLocation, ECollisionChannel::ECC_WorldDynamic, EyeParams);
	bool Trace2 = World->LineTraceSingleByChannel(Outhit, EyeLocation, LHCLocation, ECollisionChannel::ECC_WorldDynamic, EyeParams);
	bool Trace3 = World->LineTraceSingleByChannel(Outhit, EyeLocation, RHCLocation, ECollisionChannel::ECC_WorldDynamic, EyeParams);

	UE_LOG(LogTemp, Warning, TEXT("See: %d"), CanSeeHitCounter);
	/*
	// Debug stuff //////
	if (!Trace1)
	{
		UE_LOG(LogTemp, Warning, TEXT("I see camera!"));
		DrawDebugLine(World, EyeLocation, CameraLocation, FColor::Orange, false, World->DeltaTimeSeconds * 2.f);
		//CanSeeHitCounter++;
	}
	if (!Trace2)
	{
		UE_LOG(LogTemp, Warning, TEXT("I see Left hand!"));
		DrawDebugLine(World, EyeLocation, LHCLocation, FColor::Red, false, World->DeltaTimeSeconds * 2.f);
		//CanSeeHitCounter++;
	}
	if (!Trace3)
	{
		UE_LOG(LogTemp, Warning, TEXT("I see Right hand!"));
		DrawDebugLine(World, EyeLocation, RHCLocation, FColor::Cyan, false, World->DeltaTimeSeconds * 2.f);
		//CanSeeHitCounter++;
	}
	// Debug stuff //////
	*/
	if (Trace1 && Trace2 && Trace3)
	{
		CanSeeHitCounter -= 1;	// or maybe -= 2;
		if (CanSeeHitCounter < 0) { CanSeeHitCounter = 0; }
	}
	else
	{
		FVector Dif = Player->GetActorLocation() - EyeLocation;
		int DifLen = Dif.Size();
		UKismetMathLibrary::Vector_Normalize(Dif);
		FVector EV = ErrolEye->GetForwardVector();
		float Dot = FVector::DotProduct(Dif, EV);
		int ChaseGauge = 0;

		if (Dot > SeeDotThreshold)
		{
			if (!Trace1) { CanSeeHitCounter++; }
			if (!Trace2) { CanSeeHitCounter++; }
			if (!Trace3) { CanSeeHitCounter++; }
			if (CanSeeHitCounter > 25) { CanSeeHitCounter = 25; }

			if (DifLen < 300.f && CanSeeHitCounter > 3)
			{
				//UE_LOG(LogTemp, Warning, TEXT("I SEE U MOTHAFUCKA"));
				ChaseGauge = 5;
			}
			else if (Dot > 0.9f && CanSeeHitCounter > 6)
			{
				//UE_LOG(LogTemp, Warning, TEXT("I SEE U MOTHAFUCKA"));
				ChaseGauge = 4;
			}
			else if (Dot > 0.75f && CanSeeHitCounter > 10)
			{
				//UE_LOG(LogTemp, Warning, TEXT("I SEE U MOTHAFUCKA"));
				ChaseGauge = 3;
			}
			else if (Dot > 0.35f && CanSeeHitCounter > 17)
			{
				//UE_LOG(LogTemp, Warning, TEXT("I SEE U MOTHAFUCKA"));
				ChaseGauge = 2;
			}
			else if(CanSeeHitCounter > 24)
			{
				//UE_LOG(LogTemp, Warning, TEXT("I SEE U MOTHAFUCKA"));
				ChaseGauge = 1;
			}

			if (ChaseGauge != 0)
			{
				switch (State)
				{
				case ErrolState::STATE_PATROL:
					ExitPatrolState();
					EnterChaseState();
					break;
				case ErrolState::STATE_CHASE:
					//	Move to random point on sphere where player is, moving farther away depending on how high ChaseGauge is.
					break;
				}
			}
		}
		else
		{
			CanSeeHitCounter -= 1;
			if (CanSeeHitCounter < 0) { CanSeeHitCounter = 0; }
		}
	}
}

void AErrolCharacter::SetHearGauge()
{

}

void AErrolCharacter::SetSeeFlashlightGauge()
{

}

void AErrolCharacter::SetDetectionGauge()
{

}

void AErrolCharacter::ShouldChase()
{
	if (DetectionGauge > DetectionThreshold)
	{
		EnterChaseState();
		// Exit Current State
	}
}

void AErrolCharacter::UpdateSpeedBasedOnRotation()
{
	FVector Vel = GetVelocity();
	UKismetMathLibrary::Vector_Normalize(Vel);
	float Dot = FVector::DotProduct(Vel, GetActorForwardVector());
	//UE_LOG(LogTemp, Warning, TEXT("d: %f"), Dot);
	switch (State)
	{
	case ErrolState::STATE_PATROL:
		//GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed - (PatrolSpeed * Dot);
		if (Dot > 0)
		{
			GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed * Dot;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
		}
		break;
	}
}