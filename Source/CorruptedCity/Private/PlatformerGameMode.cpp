// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformerGameMode.h"
#include "CheckPoint.h"
#include "Kismet/GameplayStatics.h"
#include "PlatformerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void APlatformerGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	PCharacter = Cast<APlatformerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void APlatformerGameMode::SetNewCheckPoint(ACheckPoint* NewCheckPoint)
{
	if (NewCheckPoint)
	{
		ActiveCheckPoint = NewCheckPoint;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("New check point is unvalid"));
	}
}

void APlatformerGameMode::ReturnToLastCheckPoint()
{
	if (PCharacter->IsFakeCheckPointActive)
	{
		PCharacter->UseFakeCheckPoint();
		return;
	}
	if (PCharacter && ActiveCheckPoint)
	{
		PCharacter->GetPCharacterMovementComponent()->Velocity = FVector::ZeroVector;
		PCharacter->SetActorLocation(ActiveCheckPoint->GetActorLocation() + ActiveCheckPoint->GetActorForwardVector() * 50 + 
			ActiveCheckPoint->GetActorUpVector() * PCharacter->StandingCapsuleHalfHeight);
	}
}
