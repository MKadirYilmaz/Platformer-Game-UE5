// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CheckPoint.generated.h"

UCLASS()
class CORRUPTEDCITY_API ACheckPoint : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACheckPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void OnActivateBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnActivateEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void UpdateCheckPoint();
private:
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* CheckPointMesh;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* ActivateZone;
	

	bool CanInteractable;
	class APlatformerCharacter* PCharacter;
};
