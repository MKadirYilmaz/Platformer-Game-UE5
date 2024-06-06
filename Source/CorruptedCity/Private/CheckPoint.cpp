// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPoint.h"
#include "PlatformerGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PlatformerCharacter.h"

// Sets default values
ACheckPoint::ACheckPoint()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CheckPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckPointMesh"));
	ActivateZone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ActivateZone"));
	RootComponent = CheckPointMesh;
}

// Called when the game starts or when spawned
void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();
	ActivateZone->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoint::OnActivateBeginOverlap);
	ActivateZone->OnComponentEndOverlap.AddDynamic(this, &ACheckPoint::OnActivateEndOverlap);
	PCharacter = Cast<APlatformerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

// Called every frame
void ACheckPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACheckPoint::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACheckPoint::OnActivateBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CanInteractable = true;
	UE_LOG(LogTemp, Display, TEXT("You have entered checkpoint area!"));
	PCharacter->SetInsideCheckPoint(this);
}

void ACheckPoint::OnActivateEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CanInteractable = false;
	UE_LOG(LogTemp, Display, TEXT("You have exited checkpoint area!"));
	PCharacter->SetInsideCheckPoint(nullptr);
}

void ACheckPoint::UpdateCheckPoint()
{
	if (CanInteractable)
	{
		Cast<APlatformerGameMode>(UGameplayStatics::GetGameMode(this))->SetNewCheckPoint(this);
		UE_LOG(LogTemp, Display, TEXT("New checkpoint name: %s"), *GetActorNameOrLabel());
		PCharacter->CanCreateFakeCheckPoint = true;
	}
}



