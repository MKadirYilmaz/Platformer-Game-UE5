// Fill out your copyright notice in the Description page of Project Settings.


#include "ElevatorSystem.h"
#include "Interactable.h"

// Sets default values
AElevatorSystem::AElevatorSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ElevatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elevator Mesh"));
	RootComponent = ElevatorMesh;
}

// Called when the game starts or when spawned
void AElevatorSystem::BeginPlay()
{
	Super::BeginPlay();
	Distance = FVector::Distance(GetActorLocation(), TargetPosition);
	BeginPosition = GetActorLocation();
	if (!Button.IsEmpty())
	{
		for (int i = 0; i < Button.Num(); i++)
		{
			TiedComponents.Add(Button[i]->FindComponentByClass<UInteractable>());
		}
	}
}

// Called every frame
void AElevatorSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = 0; i < TiedComponents.Num(); i++)
	{
		if (TiedComponents[i]->GetIntWithRayTracing())
		{
			CanMove = true;
			UE_LOG(LogTemp, Warning, TEXT("CanMove is now true"));
		}
	}
	

	MoveElevator(DeltaTime);


}
void AElevatorSystem::MoveElevator(float DeltaSeconds)
{
	if (CanMove)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector DirectionVector = (TargetPosition - CurrentLocation).GetSafeNormal();

		float CurrentDistance = FVector::Distance(CurrentLocation, TargetPosition);
		
		if (CurrentDistance < 10)
		{
			UE_LOG(LogTemp, Warning, TEXT("CanMove is now false"));
			CanMove = false;
			SetActorLocation(TargetPosition);
			
			FVector Temp = BeginPosition;
			BeginPosition = TargetPosition;
			TargetPosition = Temp;
			
			
		}
		else
		{
			CurrentLocation += DirectionVector * DeltaSeconds * ElevatorSpeed;

			//UE_LOG(LogTemp, Warning, TEXT("%s"), *GetActorLocation().ToCompactString());
			SetActorLocation(CurrentLocation);
		}
	}
}

