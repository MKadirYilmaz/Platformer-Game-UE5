// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
UInteractable::UInteractable()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
}

void UInteractable::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!InteractedWithRayTracing)
	{
		CheckOverlapping();
	}
}
void UInteractable::CheckOverlapping()
{
	GetOverlappingActors(OverlappingActors);
	if (OverlappingActors.Num() != 0)
	{
		InteractedWithOverlap = true;
	}
	else
	{
		InteractedWithOverlap = false;
	}
}
void UInteractable::SetIntWithOverlap(bool NewOverlapCondition)
{
	InteractedWithOverlap = NewOverlapCondition;
}

void UInteractable::SetIntWithRayTracing(bool NewRayTraceCondition)
{
	InteractedWithRayTracing = NewRayTraceCondition;
}
