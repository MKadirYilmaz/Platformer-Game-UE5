// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Platforms.generated.h"

UCLASS()
class CORRUPTEDCITY_API APlatforms : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	APlatforms();

	UFUNCTION(BlueprintCallable)
		/*
		* Changes location every frame until reaches MaxDistance, then moves backwards until reaches starting location(loop)
		* @param DeltaTime In blueprint, plug this with Event Tick Delta Seconds.
		*/
		void ChangeLocation(float DeltaTime, bool isBothDirected = false);

	UFUNCTION(BlueprintCallable)
		/*
		* Changes rotation every frame. There is no angle limit.
		* @param DeltaTime In blueprint, plug this with Event Tick Delta Seconds.
		*/
		void ChangeRotation(float DeltaTime);


	UFUNCTION(BlueprintCallable)
		/*
		* Changes rotation every frame until reaches MaxScaleDifference, then scales backward until reaches starting scale
		* @param DeltaTime In blueprint, plug this with Event Tick Delta Seconds.
		*/
		void ChangeScale(float DeltaTime);
	UFUNCTION(BlueprintCallable)
		/*
		* Platform falls with constant speed. There is no physics calculations. Best for performance.
		*
		*/
		void PlatformFall();

	UFUNCTION(BlueprintCallable)
		/*
		* Enables gravity and physics simulations. Uses Unreal physics engine. Most realistic but worst for performance.
		*/
		void PlatformPhysicsFall();

	UFUNCTION(BlueprintCallable)
		/*
		* Uses basic physics formula. Does not calculate collision simulations only calculates gravity.
		Balanced performance.
		*/
		void PlatformArtficialPhysicsFall();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Platform Physics", meta = (AllowPrivateAccess = "true"))
		//The speed value of the current platform (Be careful when changing the value while playing)
		FVector PlatformSpeedVelocity = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, Category = "Platform Physics", meta = (AllowPrivateAccess = "true"))
		//The rotation speed value of the current platform
		FRotator PlatformRotationVelocity = FRotator(0, 0, 0);

	UPROPERTY(EditAnywhere, Category = "Platform Physics", meta = (AllowPrivateAccess = "true"))
		//The speed of changing scale value of the current platform (Be careful when changing the value while playing)
		FVector PlatformScalingVelocity = FVector(0, 0, 0);
private:

	FVector StartLocation;
	FVector StartScale;

	UPROPERTY(VisibleAnywhere)
		// Distance between the start location and the current location. (This only changes when the game starts)
		float Distance;

	UPROPERTY(EditAnywhere)
		// Max distance that the platform can move
		float MaxDistance = 250;

	UPROPERTY(VisibleAnywhere)
		// Difference between the starting scale and current scale. (This only changes when the game starts)
		float ScaleDifference;

	UPROPERTY(EditAnywhere)
		// Max difference that the platform can scale
		float MaxScaleDifference = 5;

	bool isPlatformFalling = false;
	bool isFallingWithArtificialPhysics = false;
	float TotalDeltaTime;

	UPROPERTY(EditAnywhere, Category = "Platform Physics", meta = (AllowPrivateAccess = "true"))
		float ArtificialGravity = 98.f;


public:
	UPROPERTY(EditAnywhere)
		// Base static mesh
		class UStaticMeshComponent* PlatformMesh;
	UPROPERTY(EditAnywhere)
		// If the platform is interactive, it can be used as trigger area.
		class UStaticMeshComponent* TriggerArea;
	// Returns the platform's static mesh.
	FORCEINLINE UStaticMeshComponent* GetPlatformMesh() { return PlatformMesh; }

};
