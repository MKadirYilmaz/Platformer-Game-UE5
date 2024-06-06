// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElevatorSystem.generated.h"

UCLASS()
class CORRUPTEDCITY_API AElevatorSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElevatorSystem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void MoveElevator(float DeltaSeconds);
private:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ElevatorMesh;

	UPROPERTY(EditAnywhere)
		float ElevatorSpeed;

	UPROPERTY(EditAnywhere)
		FVector TargetPosition;
	float Distance;
	UPROPERTY(EditAnywhere)
		bool CanMove;
	FVector BeginPosition;
	UPROPERTY(EditAnywhere)
		TArray<AActor*> Button;
	TArray<class UInteractable*> TiedComponents;
};
