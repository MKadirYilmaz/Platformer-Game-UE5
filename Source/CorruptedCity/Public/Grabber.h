// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CORRUPTEDCITY_API UGrabber : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGrabber();

	UFUNCTION(BlueprintCallable)
		// Creates a line trace and grabs the object colliding with that line.
		void GrabObject();

	UFUNCTION(BlueprintCallable)
		// Releases the grabbed object. Sets the attributes to default.
		void ReleaseObject();
	UFUNCTION(BlueprintCallable)
		void InteractWithObject();
	// Holds the object until player chooses to release.
	void DragObject();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere)
		// Max distance player can reach. Length of line tracing
		float MaxGrabDistance = 250;

	UPROPERTY(EditAnywhere)
	    // Line tracing scanning area radius.
		float GrabRadius = 100;

	UPROPERTY(EditAnywhere)
		// How far player will hold the object after grabbing
		float HoldDistance = 200;

	bool HasHit;
	bool HasHitInteract;
	bool HasClicked;
	bool HasReleased;

	// DeltaTime
	float Delta;

	FVector Start;
	FVector End;

	UPROPERTY(EditAnywhere)
		//This value is default 100. In script part it multiplies by 100000 to get the real force.
		float ThrowingForceParameter = 100.f;

	FHitResult HitResult;
	FHitResult HitResultInteract;

	AActor* Target;
	AActor* InteractedActor;
	class UPhysicsHandleComponent* Handler;

public:
	UFUNCTION(BlueprintCallable)
		FORCEINLINE bool GetHasHit() { return HasHit; }

	UFUNCTION(BlueprintCallable)
		FVector GetHittedComponentLocation() const;
	UFUNCTION(BlueprintCallable)
		AActor* GetHittedActor() const;

	UFUNCTION(BlueprintCallable)
		AActor* GetInteractedActor() const;
	UFUNCTION(BlueprintCallable)
		void SetHasClicked(bool b_HasClicked);
	UFUNCTION(BlueprintCallable)
		void SetHasReleased(bool b_HasReleased);

	UFUNCTION(BlueprintCallable)
	    // Rotates held object horizontally
		void RotateObjectHorizontally(float MouseWheelAxis);
	UFUNCTION(BlueprintCallable)
		// Rotates held object vertically
		void RotateObjectVertically(float MouseWheelAxis);
	UFUNCTION(BlueprintCallable)
		void EnableTargetPhysics();
	UFUNCTION(BlueprintCallable)
		// Throws the held object
		void ThrowGrabbingObject();
	
};
