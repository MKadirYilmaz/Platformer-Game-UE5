// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/StaticMeshComponent.h"



// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	Handler = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Delta = DeltaTime;
	Start = GetComponentLocation();
	End = Start + GetForwardVector() * MaxGrabDistance;

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red);
	if (HasHit && HasClicked)
	{
		if (Handler == nullptr)
			return;
		DragObject();
	}
	
}
void UGrabber::DragObject()
{
	FVector TargetLocation = Start + GetForwardVector().GetSafeNormal() * HoldDistance;
	Handler->SetTargetLocationAndRotation(TargetLocation,
		HitResult.GetComponent()->GetComponentRotation());
}

void UGrabber::SetHasClicked(bool b_HasClicked)
{
	HasClicked = b_HasClicked;
	if (b_HasClicked)
	{
		HasReleased = false;
	}
}
void UGrabber::SetHasReleased(bool b_HasReleased)
{
	HasReleased = b_HasReleased;
	if (b_HasReleased)
	{
		HasClicked = false;
	}
}

void UGrabber::GrabObject()
{
	if (Handler == nullptr)
		return;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
	HasHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End,
		FQuat::Identity, ECC_GameTraceChannel1, Sphere);
	if (HasHit)
	{
		Target = HitResult.GetActor();
		SetHasClicked(true);
		HitResult.GetComponent()->WakeAllRigidBodies();

		UE_LOG(LogTemp, Warning, TEXT("%s"), *HitResult.GetComponent()->GetComponentScale().ToCompactString());
		HitResult.GetComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
		Handler->GrabComponentAtLocationWithRotation(HitResult.GetComponent(), NAME_None,
			HitResult.ImpactPoint, HitResult.GetComponent()->GetComponentRotation());
		HitResult.GetComponent()->SetEnableGravity(false);
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no object to hold"));
	}
}
void UGrabber::ReleaseObject()
{
	if (!HasHit)
		return;

	HitResult.GetComponent()->WakeAllRigidBodies();
	Handler->ReleaseComponent();
	HitResult.GetComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
	HitResult.GetComponent()->SetEnableGravity(true);
	SetHasReleased(true);
	
}
void UGrabber::InteractWithObject()
{
	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
	HasHitInteract = GetWorld()->SweepSingleByChannel(HitResultInteract, Start, End,
		FQuat::Identity, ECC_GameTraceChannel4, Sphere);
	if (HasHitInteract)
	{
		InteractedActor = HitResultInteract.GetActor();
	}
}
void UGrabber::RotateObjectHorizontally(float MouseWheelAxis)
{
	if (!Handler || !HitResult.GetActor())
		return;
	
	HitResult.GetComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	//HitResult.GetComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
	FRotator CurrentRotation = HitResult.GetComponent()->GetComponentRotation();
	CurrentRotation.Yaw += MouseWheelAxis;
	HitResult.GetComponent()->SetRelativeRotation(CurrentRotation);;
	
	UE_LOG(LogTemp, Warning, TEXT("Now locked to yaw axis"));
	UE_LOG(LogTemp, Display, TEXT("Roll:%f Pitch:%f Yaw:%f"),CurrentRotation.Roll, CurrentRotation.Pitch, CurrentRotation.Yaw);
}
void UGrabber::RotateObjectVertically(float MouseWheelAxis)
{
	if (Handler == nullptr || HitResult.GetActor() == nullptr)
		return;

	HitResult.GetComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	FRotator CurrentRotation = HitResult.GetComponent()->GetComponentRotation();
	CurrentRotation.Roll += MouseWheelAxis;
	HitResult.GetComponent()->SetRelativeRotation(CurrentRotation);
	
	UE_LOG(LogTemp, Warning, TEXT("Now locked to roll axis"));
	UE_LOG(LogTemp, Display, TEXT("Roll:%f Pitch:%f Yaw:%f"), CurrentRotation.Roll, CurrentRotation.Pitch, CurrentRotation.Yaw);
}
void UGrabber::EnableTargetPhysics()
{
	if (!Handler || !HitResult.GetActor())
		return;
	HitResult.GetComponent()->SetSimulatePhysics(true);
}
void UGrabber::ThrowGrabbingObject()
{
	if (!Handler || !HitResult.GetActor())
		return;
	FHitResult BackUpHitResult = HitResult;
	FVector ThrowingForce = GetForwardVector() * 100000 * ThrowingForceParameter;
	UE_LOG(LogTemp, Warning, TEXT("Throwing Force = %s"), *ThrowingForce.ToCompactString());
	ReleaseObject();
	if (!BackUpHitResult.GetComponent())
	{
		UE_LOG(LogTemp, Error, TEXT("Can find the hitted component (Grabber.cpp/ThrowGrabbingObject())"));
		return;
	}
	BackUpHitResult.GetComponent()->AddForce(ThrowingForce);
}
FVector UGrabber::GetHittedComponentLocation() const
{
	if (!Handler || !HitResult.GetActor())
		return FVector::ZeroVector;
	return HitResult.GetComponent()->GetComponentLocation();
}
AActor* UGrabber::GetHittedActor() const
{
	return HitResult.GetActor();
}
AActor* UGrabber::GetInteractedActor() const
{
	return HitResultInteract.GetActor();
}