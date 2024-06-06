// Fill out your copyright notice in the Description page of Project Settings.


#include "Platforms.h"
#include "Components\StaticMeshComponent.h"
#include "TimerManager.h"


// Sets default values
APlatforms::APlatforms()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>("Platform Mesh");
	TriggerArea = CreateDefaultSubobject<UStaticMeshComponent>("Trigger Area");
	TriggerArea->SetupAttachment(PlatformMesh);
	TriggerArea->SetVisibility(false);
	TriggerArea->SetCollisionProfileName("OverlapAll");
}

// Called when the game starts or when spawned
void APlatforms::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();
	StartScale = GetActorScale();
	FTimerHandle WaitHandler;
}

// Called every frame
void APlatforms::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Distance = FVector::Distance(StartLocation, GetActorLocation());
	ScaleDifference = FVector::Distance(StartScale, GetActorScale());
	if (isFallingWithArtificialPhysics)
	{
		TotalDeltaTime += DeltaTime;
		if (TotalDeltaTime > 0.1f)
		{
			PlatformArtficialPhysicsFall();
			TotalDeltaTime = 0;
		}
	}
}
void APlatforms::ChangeLocation(float DeltaTime, bool isBothDirected)
{
	FVector CurrentLocation = GetActorLocation();
	if (!isPlatformFalling)
	{
		if (Distance > MaxDistance)
		{
			if (!isBothDirected)
			{
				FVector MoveDirection = PlatformSpeedVelocity.GetSafeNormal();
				StartLocation += MoveDirection * MaxDistance;
				SetActorLocation(StartLocation);
			}
			PlatformSpeedVelocity = -PlatformSpeedVelocity;
			return;
		}
	}
	CurrentLocation += PlatformSpeedVelocity * DeltaTime;
	SetActorLocation(CurrentLocation);
}
void APlatforms::ChangeRotation(float DeltaTime)
{
	FRotator CurrentRotation = GetActorRotation();

	CurrentRotation += PlatformRotationVelocity * DeltaTime;

	SetActorRotation(CurrentRotation);
}
void APlatforms::ChangeScale(float DeltaTime)
{
	FVector CurrentScale = GetActorScale();
	if (ScaleDifference > MaxScaleDifference)
	{
		FVector ScaleDirection = PlatformScalingVelocity.GetSafeNormal();
		StartScale += ScaleDirection * MaxScaleDifference;
		SetActorScale3D(StartScale);

		PlatformScalingVelocity = -PlatformScalingVelocity;
		return;
	}
	CurrentScale += PlatformScalingVelocity * DeltaTime;
	SetActorScale3D(CurrentScale);
}
void APlatforms::PlatformFall()
{
	PlatformSpeedVelocity = FVector(0, 0, -500);
	isPlatformFalling = true;
}
void APlatforms::PlatformPhysicsFall()
{
	isPlatformFalling = true;
	PlatformSpeedVelocity = FVector::Zero();
	PlatformMesh->SetSimulatePhysics(true);
	PlatformMesh->SetEnableGravity(true);
}
void APlatforms::PlatformArtficialPhysicsFall()
{
	isPlatformFalling = true;
	isFallingWithArtificialPhysics = true;
	PlatformSpeedVelocity.X = 0;
	PlatformSpeedVelocity.Y = 0;
	PlatformSpeedVelocity.Z -= ArtificialGravity;
}

