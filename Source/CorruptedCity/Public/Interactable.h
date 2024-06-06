// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Interactable.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CORRUPTEDCITY_API UInteractable : public UBoxComponent
{
	GENERATED_BODY()
public:
	UInteractable();

	bool GetIntWithOverlap() { return InteractedWithOverlap; }
	bool GetIntWithRayTracing() { return InteractedWithRayTracing; }
	UFUNCTION(BlueprintCallable)
		void SetIntWithOverlap(bool NewOverlapCondition);
	UFUNCTION(BlueprintCallable)
		void SetIntWithRayTracing(bool NewRayTraceCondition);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void CheckOverlapping();

private:
	bool InteractedWithOverlap;
	bool InteractedWithRayTracing;
	TArray<AActor*> OverlappingActors;
};
