// Fill out your copyright notice in the Description page of Project Settings.


#include "DetectionZone.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "StealthHealthComponent.h"

// Sets default values
ADetectionZone::ADetectionZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DetectionZoneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DetectionZoneMesh"));
	RootComponent = DetectionZoneMesh;
}

// Called when the game starts or when spawned	
void ADetectionZone::BeginPlay()
{
	Super::BeginPlay();
	DetectionZoneMesh->OnComponentBeginOverlap.AddDynamic(this, &ADetectionZone::OnHitDetection);
	DetectionZoneMesh->OnComponentEndOverlap.AddDynamic(this, &ADetectionZone::OnEndHitDetection);
}

// Called every frame
void ADetectionZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ShouldDealDamage)
	{
		UE_LOG(LogTemp, Error, TEXT("Damage applied. Target: %s"), *TargetActor->GetActorNameOrLabel());
		UGameplayStatics::ApplyDamage(TargetActor, DetectionDamageAmount * DeltaTime, UGameplayStatics::GetPlayerCharacter(this, 0)->GetInstigatorController(), this, UDamageType::StaticClass());
	}
}

void ADetectionZone::OnHitDetection(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TargetActor = OtherActor;
	ShouldDealDamage = true;
	if(TargetActor->FindComponentByClass<UStealthHealthComponent>())
		TargetActor->FindComponentByClass<UStealthHealthComponent>()->CanHeal = false;
}

void ADetectionZone::OnEndHitDetection(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ShouldDealDamage = false;
	if (TargetActor->FindComponentByClass<UStealthHealthComponent>())
	TargetActor->FindComponentByClass<UStealthHealthComponent>()->CanHeal = true;
}

