// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MotionControllerComponent.h"
#include "Misc/FrameRate.h"
#include "GameFramework/PlayerController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathVectorCommon.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "XRMotionControllerBase.h"
#include "HeadMountedDisplay.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "HandController.h"


// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Stage);

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);

	//LeftController->SetTrackingSource(EControllerHand::Left);
	//LeftController->bDisplayDeviceModel = true;
	// Create the hand mesh for visualization

	//RightController->SetTrackingSource(EControllerHand::Right);
	//RightController->bDisplayDeviceModel = true;

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestinationMarker"));
	DestinationMarker->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DestinationMarker->SetVisibility(false);

	LeftController = GetWorld()->SpawnActor<AHandController>(HandControllerClass);
	if (LeftController != nullptr)
	{
		LeftController->AttachToComponent(VRRoot, FAttachmentTransformRules::KeepRelativeTransform);
		LeftController->SetHand(EControllerHand::Left);
		LeftController->SetOwner(this);
	}

	RightController = GetWorld()->SpawnActor<AHandController>(HandControllerClass);
	if (RightController != nullptr)
	{
		RightController->AttachToComponent(VRRoot, FAttachmentTransformRules::KeepRelativeTransform);
		RightController->SetHand(EControllerHand::Right);
		RightController->SetOwner(this);
	}


}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//GetWorld()->GetFirstPlayerController()->SetControlRotation(Camera->GetComponentRotation());

	FVector NewCameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	NewCameraOffset.Z = 0;

	///////
	
	AddActorWorldOffset(NewCameraOffset);
	VRRoot->AddWorldOffset(-NewCameraOffset);

	if (bTeleportEnabled)
	{
		UpdateDestinationMarker();
	}

	if (bDodge)
	{
		Dodge();
	}

	if (bAction1 && bAction2 && !bDodge)
	{
		InterpretMCMotion();

		if (bDodge)
		{
			Dodge();
		}
		else if (bSprint)
		{
			MoveForward(1.0f);
		}
	}
}

void AVRCharacter::Dodge()
{
	SetActorLocation(FMath::VInterpTo(GetActorLocation(), DodgePos, GetWorld()->DeltaTimeSeconds, 1.0f));
}

void AVRCharacter::InterpretMCMotion()
{
	if (TickCounter % 4 == 0)	// Do this every four frames
	{
		// (x >= 0) ? x : -x

		FVector LeftOffset = LeftController->GetActorLocation() - MCLeftPos;
		FVector RightOffset = RightController->GetActorLocation() - MCRightPos;



		float LeftZOffset = LeftController->GetActorLocation().Z - MCLeftPos.Z;
		float RightZOffset = RightController->GetActorLocation().Z - MCRightPos.Z;


		//	CHECK SPRINT MOTION
		if (LeftZOffset > 0 || RightZOffset > 0)
		{
			if (RightZOffset < 0 || LeftZOffset < 0)
			{
				if (LeftZOffset < 0)
					LeftZOffset = -LeftZOffset;
				if (RightZOffset < 0)
					RightZOffset = -RightZOffset;

				if (LeftZOffset > 10 && RightZOffset > 10)
				{
					bSprint = true;
					StopSprintChecks = 0;
				}
			}
		}

		// CHECK DODGE MOTION

		float DotProd = FVector::DotProduct(LeftOffset, RightOffset);

		//UE_LOG(LogTemp, Warning, TEXT("LEFT: %f %f %f"), LeftOffset.X, LeftOffset.Y, LeftOffset.Z);
		//UE_LOG(LogTemp, Warning, TEXT("RIGHT: %f %f %f"), RightOffset.X, RightOffset.Y, RightOffset.Z);
		//UE_LOG(LogTemp, Warning, TEXT("DOTPROD: %f"), DotProd);

		if (DotProd > 120)
		{
			UE_LOG(LogTemp, Warning, TEXT("DODGE!!!!!!"));
		}

		/*
		if (DotProd > 200 && LeftZOffset < 6 && RightZOffset < 6)
		{
			FVector AddedVecs = (LeftOffset + RightOffset);
			bDodge = true;
			DodgePos = GetActorLocation() + (AddedVecs / AddedVecs.Size()) * 100;
			DodgePos.Z = GetActorLocation().Z;
			UE_LOG(LogTemp, Warning, TEXT("DOT: %f"), DotProd);
			FVector PlayerV = FVector(Camera->GetComponentLocation().X, Camera->GetComponentLocation().Y, Camera->GetComponentLocation().Z - 10);
			DrawDebugLine(GetWorld(), PlayerV, PlayerV + (LeftOffset + RightOffset) * DotProd, FColor::Turquoise, false, 1.0f);
			StopSprintChecks += StopSprintMax;
		}*/

		if (StopSprintChecks > StopSprintMax)
			bSprint = false;

		StopSprintChecks++;
		MCLeftPos = LeftController->GetActorLocation();
		MCRightPos = RightController->GetActorLocation();
	}

	if (TickCounter > 10000)
	{
		TickCounter = 0;
	}
	TickCounter++;

}

void AVRCharacter::UpdateDestinationMarker()
{
	FVector Start = RightController->GetActorLocation();
	FVector End = Start + RightController->GetActorForwardVector() * MaxTeleportDistance;

	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

	if (bHit)
	{
		if (fabsf(1 - FVector::DotProduct(HitResult.Normal, FVector(0, 0, 1))) < 0.4f)
		{
			DestinationMarker->SetVisibility(true);
			DestinationMarker->SetWorldLocation(HitResult.Location);
		}
		else
		{
			DestinationMarker->SetVisibility(false);
		}
	}
	else
	{
		DestinationMarker->SetVisibility(false);
	}

	//DestinationMarker->SetWorldLocation(CameraTraceResult.ImpactPoint);
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AVRCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &AVRCharacter::TurnRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AVRCharacter::LookUp);
	PlayerInputComponent->BindAction(TEXT("Teleport"), IE_Released, this, &AVRCharacter::BeginTeleport);
	PlayerInputComponent->BindAction(TEXT("Action1"), IE_Pressed, this, &AVRCharacter::EnableAction1);
	PlayerInputComponent->BindAction(TEXT("Action2"), IE_Pressed, this, &AVRCharacter::EnableAction2);
	PlayerInputComponent->BindAction(TEXT("Action1"), IE_Released, this, &AVRCharacter::DisableAction1);
	PlayerInputComponent->BindAction(TEXT("Action2"), IE_Released, this, &AVRCharacter::DisableAction2);
}



void AVRCharacter::MoveForward(float throttle)
{
	if (!bDodge)
	{
		if (bSprint)
		{
			AddMovementInput(Camera->GetForwardVector());
		}
		else
		{
			AddMovementInput(throttle * Camera->GetForwardVector(), 0.5f);
		}
	}
}

void AVRCharacter::MoveRight(float throttle)
{
	if (!bDodge)
	{
		AddMovementInput(throttle * Camera->GetRightVector(), 0.5f);
	}
}

void AVRCharacter::TurnRight(float throttle)
{
	AddControllerYawInput(throttle);
}

void AVRCharacter::LookUp(float throttle)
{
	AddControllerPitchInput(-throttle);
}

void AVRCharacter::BeginTeleport()
{
	if (DestinationMarker->IsVisible())
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC != nullptr)
		{
			PC->PlayerCameraManager->StartCameraFade(0, 1, TeleportFadeTime, FLinearColor::Black);
		}

		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AVRCharacter::FinishTeleport, TeleportFadeTime / 2);
	}
}

void AVRCharacter::FinishTeleport()
{
	FVector TeleportLocation = DestinationMarker->GetComponentLocation();
	TeleportLocation.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SetActorLocation(TeleportLocation);
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC != nullptr)
	{
		PC->PlayerCameraManager->StartCameraFade(1, 0, TeleportFadeTime, FLinearColor::Black);
	}
}

void AVRCharacter::EnableAction1()
{
	bAction1 = true;
	UE_LOG(LogTemp, Warning, TEXT("Action1"));
}

void AVRCharacter::EnableAction2()
{
	bAction2 = true;
	UE_LOG(LogTemp, Warning, TEXT("Action2"));
}

void AVRCharacter::DisableAction1()
{
	bAction1 = false;
	bSprint = false;
}

void AVRCharacter::DisableAction2()
{
	bAction2 = false;
	bSprint = false;
}

