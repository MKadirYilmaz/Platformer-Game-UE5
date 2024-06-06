// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StealthHealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CORRUPTEDCITY_API UStealthHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStealthHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool CanHeal;

private:
	UPROPERTY(EditAnywhere)
		float MaxStealthHealth;
	UPROPERTY(VisibleAnywhere)
		float CurrentStealthHealth;
	UPROPERTY(EditAnywhere)
		float HealSpeed = 100.f;
	class APlatformerGameMode* PlatformerGameMode;
	UFUNCTION()
		void OnDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser);
	
};
