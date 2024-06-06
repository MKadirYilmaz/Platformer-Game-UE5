// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlatformerGameMode.h"

// Sets default values for this component's properties
UStealthHealthComponent::UStealthHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStealthHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentStealthHealth = MaxStealthHealth;
	PlatformerGameMode = Cast<APlatformerGameMode>(UGameplayStatics::GetGameMode(this));

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UStealthHealthComponent::OnDamageTaken);
	// ...
	
}


// Called every frame
void UStealthHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CanHeal && CurrentStealthHealth < MaxStealthHealth)
	{
		CurrentStealthHealth += HealSpeed * DeltaTime;
		if (CurrentStealthHealth > MaxStealthHealth)
			CurrentStealthHealth = MaxStealthHealth;
		UE_LOG(LogTemp, Warning, TEXT("New health: %f"), CurrentStealthHealth);
	}
	// ...
}

void UStealthHealthComponent::OnDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser)
{
	CurrentStealthHealth -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("New health: %f"), CurrentStealthHealth);
	if (CurrentStealthHealth <= 0 && PlatformerGameMode)
	{
		PlatformerGameMode->ReturnToLastCheckPoint();
		CurrentStealthHealth = MaxStealthHealth;
	}
}

