// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollisionQueryParams.h"
#include "Roach.generated.h"

UCLASS()
class ARCHITECTUREEXPLORER_API ARoach : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoach();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* RoachRoot;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* RoachMesh;

	void Move(float DeltaTime);

	// For making the roach parallel to the plane it is walking on
	void ZeroRoll(float DeltaTime);

	bool CheckFront();
	bool CheckDown();

	void TraverseCorner(float DeltaTime, bool Down);
	
	void Swerve(float DeltaTime);

private:

	float RoachLength = 13.f;
	float RoachWidth = 8.f;
	float Speed = 200.f;

	int t = 0;

	bool TraversingUpCorner;
	bool TraversingDownCorner;
	
	FVector TurnPoint = FVector::ZeroVector;


	FHitResult HitResult;
	//	I think for all of the classes I am doing line traces in, I want to check if I am querying too many things
	FCollisionQueryParams RoachParams;
	
};
