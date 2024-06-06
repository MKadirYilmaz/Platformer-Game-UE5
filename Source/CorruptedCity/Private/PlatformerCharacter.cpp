// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Grabber.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "StealthHealthComponent.h"
#include "CheckPoint.h"


// Sets default values
APlatformerCharacter::APlatformerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCamera->SetupAttachment(RootComponent);
	FirstPersonCamera->bUsePawnControlRotation = true;

	InteractHandler = CreateDefaultSubobject<UGrabber>(TEXT("Interaction Handler"));
	InteractHandler->SetupAttachment(FirstPersonCamera);

	PhysicsHandler = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Physics Handle"));

	PlayerStealthHealth = CreateDefaultSubobject<UStealthHealthComponent>(TEXT("Stealth Health"));
}

// Called when the game starts or when spawned
void APlatformerCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = GetController();
	MovementComponent = GetCharacterMovement();
	ChangeMovementState(0);
	StandingCapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	StandingCameraZOffset = FirstPersonCamera->GetRelativeLocation().Z;
	MainGravityScale = MovementComponent->GravityScale;
	MainFOVScale = FirstPersonCamera->FieldOfView;
}

// Called every frame
void APlatformerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DeltaSeconds = DeltaTime;
	CalculateRealHorizontalVelocity();
	if (IsWallRunning)
	{
		UpdateWallRun();
	}
	if (ShouldSlide)
	{
		SlideUpdate();
	}
	if (CurrentMovementState == 2 && CanStand())
	{
		ResolveMovementState();
	}

	AutoClimbingSystem();
	if (IsHanging)
	{
		ClimbUpdate();
	}	
	WalkingCalculations();
	if (IsGrappling)
	{
		ExecuteGrappleSystem();
	}
	if (CanSprint())
	{
		ChangeMovementState(1);
	}
}

void APlatformerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	CanWallRun = true;
	CurrentJumpCount = 0;
	UE_LOG(LogTemp, Warning, TEXT("Can Wall Run"));
	HasDashed = false;
	if (CrouchKeyPressed && MovementComponent->Velocity.Size() > CrouchingSpeed + 100)
	{
		ChangeMovementState(3);
	}
	PrevLedgeComponent = nullptr;

}

// Called to bind functionality to input
void APlatformerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("Look Up / Down Mouse"), this, &APlatformerCharacter::CharacterVerticalRotation);
	PlayerInputComponent->BindAxis(TEXT("Turn Right / Left Mouse"), this, &APlatformerCharacter::CharacterHorizontalRotation);
	PlayerInputComponent->BindAxis(TEXT("Move Forward / Backward"), this, &APlatformerCharacter::CharacterLocalForwardMove);
	PlayerInputComponent->BindAxis(TEXT("Move Right / Left"), this, &APlatformerCharacter::CharacterLocalRightMove);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &APlatformerCharacter::JumpingSystem);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &APlatformerCharacter::StopJumping);

	PlayerInputComponent->BindAction(TEXT("LeftMouseClick"), IE_Pressed, this, &APlatformerCharacter::GrabObject_Special);
	PlayerInputComponent->BindAction(TEXT("LeftMouseClick"), IE_Released, this, &APlatformerCharacter::ReleaseObject_Special);

	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &APlatformerCharacter::StartRunning);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &APlatformerCharacter::EndRunning);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &APlatformerCharacter::StartCrouching);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &APlatformerCharacter::EndCrouching);

	PlayerInputComponent->BindAction(TEXT("Dashing"), IE_Pressed, this, &APlatformerCharacter::StartDashing);
	PlayerInputComponent->BindAction(TEXT("Dashing"), IE_Released, this, &APlatformerCharacter::ExecuteDashing);

	PlayerInputComponent->BindAction(TEXT("SpecialSkill"), IE_Pressed, this, &APlatformerCharacter::CheckGrappleConditions);
	PlayerInputComponent->BindAction(TEXT("SpecialSkill"), IE_Released, this, &APlatformerCharacter::FinishGrappling);

	PlayerInputComponent->BindAction(TEXT("SecondarySkill"), IE_Pressed, this, &APlatformerCharacter::HoldEdge);
	PlayerInputComponent->BindAction(TEXT("SecondarySkill"), IE_Released, this, &APlatformerCharacter::ReleaseHoldEdge);

	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &APlatformerCharacter::CheckAllInteractables);

	PlayerInputComponent->BindAction(TEXT("SkillSlot_1"), IE_Pressed, this, &APlatformerCharacter::CreateFakeCheckPoint);
}
void APlatformerCharacter::CharacterHorizontalRotation(float Value)
{
	AddControllerYawInput(Value * MouseRotationSensitivity);
}
void APlatformerCharacter::CharacterVerticalRotation(float Value)
{
	AddControllerPitchInput(Value * MouseRotationSensitivity);
}
void APlatformerCharacter::CharacterLocalForwardMove(float Value)
{
	ForwardAxis = Value;
	if (CurrentMovementState == 3 || TryingToDash)
		return;
	AddMovementInput(GetActorForwardVector(), Value);
}
void APlatformerCharacter::CharacterLocalRightMove(float Value)
{
	RightAxis = Value;
	if (CurrentMovementState == 3 || TryingToDash)
		return;
	AddMovementInput(GetActorRightVector(), Value);
}

void APlatformerCharacter::JumpingSystem()
{
	if (IsHanging)
	{
		if (DistanceBetweenLedges > 0)
		{
			LaunchCharacter((FirstPersonCamera->GetForwardVector() + FVector(0, 0, 1)) * DistanceBetweenLedges * 1.5f, false, true);
		}
		else
		{
			LaunchCharacter(FVector(0, 0, MovementComponent->JumpZVelocity * 1.25f), false, true);
		}
		
		IsHanging = false;
		return;
	}
	if (CurrentJumpCount < MaxJumpCount)
	{
		ResolveMovementState();
		CurrentJumpCount++;

		LaunchCharacter(FindLauchVelocity(), false, true);
		if (IsWallRunning)
		{
			EndReason = EndWallRunReason::JumpOfWall;
			EndWallRun(EndReason);
		}
	}
}

void APlatformerCharacter::StopJump()
{
	StopJumping();
}

void APlatformerCharacter::GrabObject_Special()
{
	InteractHandler->GrabObject();
}

void APlatformerCharacter::ReleaseObject_Special()
{
	InteractHandler->ReleaseObject();
}

void APlatformerCharacter::StartRunning()
{
	SprintKeyPressed = true;
	if (CurrentMovementState == 0)
	{
		ChangeMovementState(1);
	}
	else if (CurrentMovementState == 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Running Started (Resolve)"));
		ResolveMovementState();
	}
}

void APlatformerCharacter::EndRunning()
{
	SprintKeyPressed = false;
	if (CurrentMovementState == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Running Ended (Resolve)"));
		ResolveMovementState();
	}
}

void APlatformerCharacter::StartCrouching()
{
	CrouchKeyPressed = true;
	if (IsWallRunning)
	{
		EndWallRun(FallOfWall);
		return;
	}
	if (CurrentMovementState == 0 || CurrentMovementState == 1) // Walking or running
	{
		ChangeMovementState(3);
	}
	else if (CurrentMovementState == 3)
	{
		ChangeMovementState(2);
	}
}

void APlatformerCharacter::EndCrouching()
{
	CrouchKeyPressed = false;
	if (CurrentMovementState == 2 || CurrentMovementState == 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Crouching or sliding ended (Resolve)"));
		ResolveMovementState();
	}
}

void APlatformerCharacter::CheckWallRunConditions(const FVector& HitImpactNormal, UPrimitiveComponent* OtherComp)
{
	if (!CrouchKeyPressed && !IsWallRunning && CanSurfaceBeWallRun(HitImpactNormal, OtherComp) && MovementComponent->IsFalling())
	{
		WallRunDirection = FindWallRunSideAndDirection(HitImpactNormal, CurrentWallRunSide);
		LastSide = CurrentWallRunSide;
		if (AreRequiredKeysDown())
		{
			float CrossAbsZValue = FVector::CrossProduct(GetActorForwardVector(), HitImpactNormal).GetAbs().Z;
			if (CrossAbsZValue >= 0.5f && CanWallRun && RealHorizontalVelocity > MinWallRunStartSpeed)
			{
				BeginWallRun();
			}
		}
	}
}

bool APlatformerCharacter::CanSurfaceBeWallRun(const FVector& SurfaceNormal, UPrimitiveComponent* WallComponent)
{
	if (!MovementComponent || WallComponent->ComponentHasTag(CantWallRunnableTags) || SurfaceNormal.Z < -0.05)
		return false;
	else
	{
		FVector Vector = FVector(SurfaceNormal.X, SurfaceNormal.Y, 0);
		float Slope = FVector::DotProduct(SurfaceNormal, Vector.GetSafeNormal());
		float Angle = FMath::Acos(Slope);
		float WalkableAngle = MovementComponent->GetWalkableFloorAngle();
		return Angle < WalkableAngle;
	}
}

FVector APlatformerCharacter::FindWallRunSideAndDirection(const FVector& WallNormal, WallRunSide& out_Current)
{
	FVector2D WallNormal2D = FVector2D(WallNormal.X, WallNormal.Y);
	FVector2D ActorRightVector2D = FVector2D(GetActorRightVector().X, GetActorRightVector().Y);
	float DotSize = FVector2D::DotProduct(WallNormal2D, ActorRightVector2D);
	if (DotSize > 0)
	{
		out_Current = WallRunSide::Right;
		return FVector::CrossProduct(WallNormal, FVector(0, 0, 1));
	}
	else
	{
		out_Current = WallRunSide::Left;
		return FVector::CrossProduct(WallNormal, FVector(0, 0, -1));
	}
}

bool APlatformerCharacter::AreRequiredKeysDown()
{
	return ForwardAxis > 0.1f;
}

void APlatformerCharacter::CalculateRealHorizontalVelocity()
{
	if (FrameNum == 0)
	{
		ActorLocation1 = FVector2D(GetActorLocation().X, GetActorLocation().Y);
	}
	if (FrameNum == 1)
	{
		ActorLocation2 = FVector2D(GetActorLocation().X, GetActorLocation().Y);
		if (DeltaSeconds != 0)
		{
			float Displacement = FVector2D::Distance(ActorLocation1, ActorLocation2);
			RealHorizontalVelocity = Displacement / DeltaSeconds;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Delta seconds is 0, cannot divide displacement with 0 (RealVelocity)"));
		}
	}
	FrameNum++;
	if (FrameNum == 2)
		FrameNum = 0;
}

void APlatformerCharacter::BeginWallRun()
{
	UE_LOG(LogTemp, Warning, TEXT("BeginWallRun"));
	MovementComponent->Velocity = FVector(MovementComponent->Velocity.X, MovementComponent->Velocity.Y, 0);
	ChangeMovementState(1);
	bUseControllerRotationYaw = false;
	IsWallRunning = true;
	HasDashed = false;
	MovementComponent->AirControl = 1.f;
	MovementComponent->GravityScale = WallRunGravity;
	CurrentJumpCount = 0;
	MovementComponent->SetPlaneConstraintNormal(FVector(0, 0, 1));

	StartCameraTilt();
}

void APlatformerCharacter::UpdateWallRun()
{
	UE_LOG(LogTemp, Warning, TEXT("MaxSpeed = %f, CurrentSpeed = %f"), MovementComponent->MaxWalkSpeed, MovementComponent->Velocity.Size());
	FVector LookLocation = WallRunDirection;
	FRotator LookRotation = LookLocation.Rotation();
	SetActorRotation(LookRotation);
	if (MovementComponent->Velocity.Size() > RunningSpeed)
	{
		if (CurrentMovementState == Walking) // If player is not running but it's speed is higher than normal running speed. Change movement speed to running
		{
			ChangeMovementState(1);
		}
		MovementComponent->Velocity = MovementComponent->Velocity.GetSafeNormal() * RunningSpeed; // Lower the speed to normal running speed
	}

	if (!MovementComponent->IsFalling())
	{
		EndReason = EndWallRunReason::LandOnSurface;
		EndWallRun(EndReason);
	}
	if (AreRequiredKeysDown())
	{
		FVector Value1;
		if (CurrentWallRunSide == 0) //Right
		{
			Value1 = FVector(0, 0, 1);
		}
		else //Left
		{
			Value1 = FVector(0, 0, -1);
		}
		FVector EndLocation = (FVector::CrossProduct(WallRunDirection, Value1) * 200) + GetActorLocation();
		bool IsHitted = GetWorld()->LineTraceSingleByChannel(WallRunHitResult, GetActorLocation(), EndLocation, ECC_Visibility);
		if (IsHitted)
		{
			bool FreeLookCheck = FVector::CrossProduct(FirstPersonCamera->GetForwardVector(),
				WallRunHitResult.ImpactNormal).GetAbs().Z >= 0.25f;
			if (FreeLookCheck)
			{
				FindWallRunSideAndDirection(WallRunHitResult.ImpactNormal, CurrentWallRunSide);
				if (CurrentWallRunSide == LastSide)
				{
					WallRunDirection = FindWallRunSideAndDirection(WallRunHitResult.ImpactNormal, CurrentWallRunSide);
					if (RealHorizontalVelocity > MinWallRunContinueSpeed)
					{
						AddMovementInput(WallRunHitResult.ImpactNormal * -1.f);
					}
					else
					{
						EndReason = EndWallRunReason::FallOfWall;
						EndWallRun(EndReason);
					}
				}
				else
				{
					EndReason = EndWallRunReason::FallOfWall;
					EndWallRun(EndReason);
				}
			}
			else
			{
				EndReason = EndWallRunReason::FallOfWall;
				EndWallRun(EndReason);
			}
		}
		else
		{
			EndReason = EndWallRunReason::FallOfWall;
			EndWallRun(EndReason);
		}
	}
	else
	{
		EndReason = EndWallRunReason::FallOfWall;
		EndWallRun(EndReason);
	}
}

void APlatformerCharacter::EndWallRun(const EndWallRunReason& EndRunReason)
{
	UE_LOG(LogTemp, Warning, TEXT("EndWallRun"));
	bUseControllerRotationYaw = true;
	if (EndRunReason == 0) // FallOfWall
	{
		CanWallRun = false;
		CurrentJumpCount = MaxJumpCount - 1;
	}
	else if (EndRunReason == 1)// JumpOffWall
	{
		CanWallRun = true;
		CurrentJumpCount = 1;
	}
	else
	{
		CanWallRun = true;
		CurrentJumpCount = 0;
	}
	MovementComponent->AirControl = 0.5f;
	MovementComponent->GravityScale = 1.5f;
	MovementComponent->SetPlaneConstraintNormal(FVector(0, 0, 0));
	IsWallRunning = false;
	EndCameraTilt();
}

FVector APlatformerCharacter::FindLauchVelocity() const
{
	FVector LaunchDirection;
	if (IsWallRunning)
	{
		FVector CrossProductParam;
		if (LastSide == 0) //Right
		{
			CrossProductParam = FVector(0, 0, -1);
		}
		else // Left
		{
			CrossProductParam = FVector(0, 0, 1);
		}
		LaunchDirection = FVector::CrossProduct(WallRunDirection, CrossProductParam) * 0.5f;
	}
	else
	{
		if (MovementComponent->IsFalling())
		{
			LaunchDirection = ((RightAxis * GetActorRightVector()) + (ForwardAxis * GetActorForwardVector())) * 0.35f;

		}
	}
	return (LaunchDirection + FVector(0, 0, 1)) * MovementComponent->JumpZVelocity;
}

void APlatformerCharacter::CameraTiltEffectValue(float CameraParam)
{
	float CameraRoll;
	if (CurrentWallRunSide == 0) //Right
	{
		CameraRoll = CameraParam;
	}
	else
	{
		CameraRoll = -CameraParam;
	}
	FRotator CurrentRotation = GetController()->GetControlRotation();
	GetController()->SetControlRotation(FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, CameraRoll));
}

void APlatformerCharacter::ChangeMovementState(int32 MovementStateIndex)
{
	if (MovementStateIndex != CurrentMovementState)
	{
		PrevMovementState = CurrentMovementState;
		switch (MovementStateIndex)
		{
		case 0:
			CurrentMovementState = MovementState::Walking;
			break;
		case 1:
			CurrentMovementState = MovementState::Running;
			break;
		case 2:
			CurrentMovementState = MovementState::Crouching;
			break;
		case 3:
			CurrentMovementState = MovementState::Sliding;
			break;
		default:
			break;
		}
		UE_LOG(LogTemp, Display, TEXT("%d"), CurrentMovementState);
		OnMovementStateChange(PrevMovementState);
		if (CurrentMovementState == 0 || CurrentMovementState == 1)
		{
			EndCrouchEffect();
		}
		else if (CurrentMovementState == 2)
		{
			BeginCrouchEffect();
		}
		else
		{
			BeginCrouchEffect();
			BeginSlide();
		}

	}
}

void APlatformerCharacter::OnMovementStateChange(int32 PrevMovementStateIndex)
{
	switch (CurrentMovementState)
	{
	case Walking:
		MovementComponent->MaxWalkSpeed = WalkingSpeed;
		break;
	case Running:
		MovementComponent->MaxWalkSpeed = RunningSpeed;
		break;
	case Crouching:
		MovementComponent->MaxWalkSpeed = CrouchingSpeed;
		break;
	case Sliding:
		MovementComponent->MaxWalkSpeed = 1000.f;
		break;
	default:
		break;
	}
	if (PrevMovementStateIndex == 3)
	{
		MovementComponent->GroundFriction = 8.f;
		MovementComponent->BrakingDecelerationWalking = 2048.f;
		UE_LOG(LogTemp, Warning, TEXT("Slide has ended because prev state was sliding"));
		EndSlide();
	}
	if (CurrentMovementState == 3)
	{
		MovementComponent->GroundFriction = 0.f;
		MovementComponent->BrakingDecelerationWalking = 450.f;
	}
}

bool APlatformerCharacter::CanSprint()
{
	if (!SprintKeyPressed)
	{
		return false;
	}
	else
	{
		return (!MovementComponent->IsFalling()) && CanStand();
	}
}

bool APlatformerCharacter::CanStand()
{
	if (CrouchKeyPressed)
	{
		return false;
	}
	else
	{
		FVector Start = FVector(GetActorLocation().X, GetActorLocation().Y,
			GetActorLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		FVector End = FVector(Start.X, Start.Y, Start.Z + StandingCapsuleHalfHeight * 2);
		bool IsHitted = GetWorld()->LineTraceSingleByChannel(CrouchingHitResult, Start, End, ECC_Visibility);
		return !IsHitted;
	}
}

void APlatformerCharacter::ResolveMovementState()
{
	int32 MovementIndex;
	if (CanSprint())
	{
		//Running
		MovementIndex = 1;
	}
	else
	{
		if (CanStand())
		{
			//Walking
			MovementIndex = 0;
		}
		else
		{
			//Crouching
			MovementIndex = 2;
		}
	}
	ChangeMovementState(MovementIndex);
}

void APlatformerCharacter::UpdateCrouchSystem(float HalfHeightParam)
{
	float NewHalfHeight = UKismetMathLibrary::MapRangeClamped(HalfHeightParam, 0, 1, 35, StandingCapsuleHalfHeight);
	GetCapsuleComponent()->SetCapsuleHalfHeight(NewHalfHeight);
	float NewCameraZOffset = UKismetMathLibrary::MapRangeClamped(HalfHeightParam, 0, 1, 25, StandingCameraZOffset);
	FVector CameraRelativeLoc = FirstPersonCamera->GetRelativeLocation();
	FirstPersonCamera->SetRelativeLocation(FVector(CameraRelativeLoc.X, CameraRelativeLoc.Y, NewCameraZOffset));
}

void APlatformerCharacter::BeginSlide()
{
	ShouldSlide = true;
	IsSliding = true;
	if (MovementComponent->Velocity.Size() < RunningSpeed && !MovementComponent->IsFalling())
		MovementComponent->Velocity *= 1.4;
	else if (MovementComponent->Velocity.Size() > RunningSpeed * 2 && !MovementComponent->IsFalling())
		MovementComponent->Velocity = MovementComponent->Velocity.GetSafeNormal() * RunningSpeed * 2;
	UE_LOG(LogTemp, Warning, TEXT("Sliding speed: %f"), MovementComponent->Velocity.Size());
	BeginSlideCameraTilt();
	UE_LOG(LogTemp, Warning, TEXT("BeginSlide"));

}

void APlatformerCharacter::EndSlide()
{
	ShouldSlide = false;
	IsSliding = false;
	EndSlideCameraTilt();
	UE_LOG(LogTemp, Warning, TEXT("EndSlide"));
}

FVector APlatformerCharacter::CalculateFloorInfluence(const FVector& FloorNormal)
{
	if (UKismetMathLibrary::EqualEqual_VectorVector(FloorNormal, GetActorUpVector(), 0.001))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Floor pushing vector is zero"));
		return FVector::ZeroVector;
	}
	else
	{
		FVector CrossResult = FVector::CrossProduct(FloorNormal, FVector::CrossProduct(FloorNormal, GetActorUpVector()));
		float FloorInfluenceDot = FVector::DotProduct(FloorNormal, GetActorUpVector());
		FVector PushingVector = CrossResult.GetSafeNormal() *
			(350000 * UKismetMathLibrary::FClamp((1 - FloorInfluenceDot), 0, 1));
		return PushingVector;
	}
}

void APlatformerCharacter::SlideUpdate()
{
	float CosForwardVelocity = FVector::DotProduct(GetActorForwardVector(), MovementComponent->Velocity) /
		(GetActorForwardVector().Size() * MovementComponent->Velocity.Size());
	FVector VelocityForward = (MovementComponent->Velocity * CosForwardVelocity);
	ForwardVelocity = MovementComponent->Velocity.Size() * GetActorForwardVector();
	FVector SideForce = (GetActorRightVector() * RightAxis).GetSafeNormal() * 70;
	//UE_LOG(LogTemp, Warning, TEXT("Cos: %f, VF: %s"), CosForwardVelocity, *VelocityForward.ToCompactString());
	if (CosForwardVelocity > 0) // A little adjustment. Changes sliding velocity rotation based on player's looking direction
	{
		MovementComponent->Velocity = FMath::VInterpTo(MovementComponent->Velocity, ForwardVelocity, DeltaSeconds, 1.25f);
	}

	MovementComponent->AddForce(CalculateFloorInfluence(GetCharacterMovement()->CurrentFloor.HitResult.Normal) + SideForce * VelocityForward.Size());
	if (GetVelocity().Size() > 2000)
	{

		UE_LOG(LogTemp, Warning, TEXT("Velocity is bigger than 2000, velocity:%f"), GetVelocity().Size());
		FVector CurrentVelocity = GetVelocity();
		CurrentVelocity.Normalize(0.0001);

		MovementComponent->Velocity = CurrentVelocity * 2000;
	}
	if (GetVelocity().Size() < CrouchingSpeed + 100)
	{
		UE_LOG(LogTemp, Warning, TEXT("%f (CurrentSpeed) lower than %f (Crouching + 50)"), GetVelocity().Size(), CrouchingSpeed + 50);
		ResolveMovementState();
	}
}

void APlatformerCharacter::SlideCameraTiltUpdate(float TiltValue)
{
	FRotator ControllerRotation = GetController()->GetControlRotation();
	GetController()->SetControlRotation(FRotator(ControllerRotation.Pitch, ControllerRotation.Yaw, TiltValue));
}

void APlatformerCharacter::AutoClimbingSystem()
{
	if (MovementComponent->IsFalling() && !IsWallRunning && !IsHanging)
	{
		FCollisionShape SphereCollision = FCollisionShape::MakeSphere(4.f);
		FVector _Start = GetActorLocation();
		FVector _End = _Start + GetActorForwardVector() * 60;
		bool IsThereALedge = GetWorld()->SweepSingleByChannel(AutoClimbingResult_1, _Start, _End, GetActorRotation().Quaternion(), ECC_GameTraceChannel3,
			SphereCollision);
		if (IsThereALedge)
		{
			UStaticMeshComponent* _CurrentLedge = Cast<UStaticMeshComponent>(AutoClimbingResult_1.GetComponent());
			DirectionToLedge = (AutoClimbingResult_1.ImpactPoint - GetActorLocation()).GetSafeNormal();
			PossibleMovableDirection = FVector::CrossProduct(AutoClimbingResult_1.ImpactNormal, _CurrentLedge->GetUpVector());
			if (PrevLedgeComponent)
			{
				if (_CurrentLedge == PrevLedgeComponent)
				{
					if (MovementComponent->Velocity.Z < 0.f)
					{
						HangingLocation = GetActorLocation();
						CurrentJumpCount = 0;
						IsHanging = true;
						return;
					}
				}
				else
				{
					HangingLocation = GetActorLocation();
					IsHanging = true;
					PrevLedgeComponent = _CurrentLedge;
					CurrentJumpCount = 0;
					return;
				}
			}
			else
			{
				HangingLocation = GetActorLocation();
				IsHanging = true;
				PrevLedgeComponent = _CurrentLedge;
				CurrentJumpCount = 0;
				return;
			}
		}
		
	}
}

void APlatformerCharacter::ClimbUpdate()
{
	if (ForwardAxis < 0 || CrouchKeyPressed)
	{
		IsHanging = false;
		return;
	}
	else
	{
		AddMovementInput(PossibleMovableDirection, RightAxis);
		FCollisionShape SphereCollision = FCollisionShape::MakeSphere(4.f);
		bool IsPlayerLookingAtALedge = GetWorld()->SweepSingleByChannel(AutoClimbingResult_2, GetActorLocation(),
			GetActorLocation() + (FirstPersonCamera->GetForwardVector() * 400), GetActorRotation().Quaternion(), ECC_GameTraceChannel3, SphereCollision);
		if (IsPlayerLookingAtALedge)
		{
			UE_LOG(LogTemp, Display, TEXT("You are looking at a ledge %s"), *AutoClimbingResult_2.GetActor()->GetActorNameOrLabel());
			if (AutoClimbingResult_2.GetComponent() != PrevLedgeComponent)
			{
				DistanceBetweenLedges = FVector::Distance(GetActorLocation(), AutoClimbingResult_2.GetComponent()->GetComponentLocation());
			}
			else
			{
				DistanceBetweenLedges = 0;
			}
		}
		else
		{
			DistanceBetweenLedges = 0;
		}
		
		HangingLocation = HangingLocation + (PossibleMovableDirection * RightAxis);
		bool _IsBlocked = SetActorLocation(HangingLocation, true);
		MovementComponent->Velocity = FVector::ZeroVector;
		bool IsLedgeThere = GetWorld()->SweepSingleByChannel(AutoClimbingResult_3, GetActorLocation(),
			GetActorLocation() + DirectionToLedge * 65, GetActorRotation().Quaternion(), ECC_GameTraceChannel3, SphereCollision);
		if (!IsLedgeThere)
			IsHanging = false;
		else
		{
			if (PrevLedgeComponent != AutoClimbingResult_3.GetComponent())
			{
				PrevLedgeComponent = Cast<UStaticMeshComponent>(AutoClimbingResult_3.GetComponent());
			}
			DirectionToLedge = (AutoClimbingResult_3.ImpactPoint - GetActorLocation()).GetSafeNormal();
			PossibleMovableDirection = FVector::CrossProduct(AutoClimbingResult_3.ImpactNormal, 
				AutoClimbingResult_3.GetComponent()->GetUpVector());
		}
	}
}

void APlatformerCharacter::HoldEdge()
{
	CanHoldEdge = true;
}

void APlatformerCharacter::ReleaseHoldEdge()
{
	CanHoldEdge = false;
}

void APlatformerCharacter::StartDashing()
{
	if (CanDash && !HasDashed && MovementComponent->IsFalling() && !IsWallRunning)
	{
		UE_LOG(LogTemp, Display, TEXT("Started dashing"));
		MovementComponent->GravityScale = 0.f;
		MovementComponent->Velocity = FVector::ZeroVector;
		TryingToDash = true;
		StartDashingCameraEffect();
		GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlatformerCharacter::ExecuteDashing, MaxDashWaitTime, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot dash"));
		TryingToDash = false;
	}
}

void APlatformerCharacter::ExecuteDashing()
{
	EndDashingCameraEffect();
	if (TryingToDash && MovementComponent->IsFalling() && !IsWallRunning)
	{
		MovementComponent->GravityScale = MainGravityScale;
		FVector LaunchVector = FirstPersonCamera->GetForwardVector() * DashingForceParameter;
		LaunchCharacter(LaunchVector, true, true);
		HasDashed = true;
		TryingToDash = false;
		FirstPersonCamera->FieldOfView = MainFOVScale;
	}
}

void APlatformerCharacter::UpdateDashFOV(float FOV_Value)
{
	FirstPersonCamera->FieldOfView = FOV_Value;
}

void APlatformerCharacter::CheckGrappleConditions()
{
	FCollisionShape ScanSphere = FCollisionShape::MakeSphere(300);
	FVector Start = GetActorLocation();
	FVector End = Start + (FirstPersonCamera->GetForwardVector() * GrappleMaxDistance);
	FHitResult _PathResult;
	bool _IsPathBlocked = GetWorld()->LineTraceSingleByChannel(_PathResult, Start, End, ECC_Visibility);
	bool CanGrapple = GetWorld()->SweepSingleByChannel(GrappleHitResult, Start, End,
		FirstPersonCamera->GetComponentRotation().Quaternion(), ECC_GameTraceChannel2, ScanSphere);
	if (CanGrapple)
		UE_LOG(LogTemp, Display, TEXT("Can grapple"));
	if (_IsPathBlocked && CanGrapple)
	{
		if (_PathResult.GetActor() == GrappleHitResult.GetActor())
		{
			GrappleTargetLocation = GrappleHitResult.ImpactPoint;
			MovementComponent->GravityScale = 0.5f;
			IsGrappling = true;
		}
	}
	else if (CanGrapple)
	{
		GrappleTargetLocation = GrappleHitResult.ImpactPoint;
		MovementComponent->GravityScale = 0.5f;
		IsGrappling = true;
	}
}

void APlatformerCharacter::ExecuteGrappleSystem()
{
	GrappleRopeSystem(IsGrappling);
	if (GetActorLocation().Z > GrappleTargetLocation.Z || FVector::Distance(GrappleTargetLocation, GetActorLocation()) > GrappleMaxDistance + 10)
	{
		FinishGrappling();
		return;
	}
	FVector GrapplingDirection = GrappleTargetLocation - GetActorLocation();
	FVector DirectionUnitVector = GrapplingDirection.GetSafeNormal();
	FVector GrappleLaunchForce = (DirectionUnitVector + (RightAxis * GetActorRightVector()) * 0.3f).GetSafeNormal() * 250000;

	MovementComponent->AddForce(GrappleLaunchForce);
}

void APlatformerCharacter::FinishGrappling()
{
	if (IsGrappling)
	{
		MovementComponent->GravityScale = MainGravityScale;
		IsGrappling = false;
		GrappleRopeSystem(IsGrappling);
	}
}

void APlatformerCharacter::WalkingCalculations()
{
	if (CurrentMovementState == 3 || (MovementComponent->IsFalling() && !IsWallRunning))
		return;
	if (CurrentFrame == 0)
	{
		FirstLoc = GetActorLocation();
		CurrentFrame++;
		return;
	}
	else
	{
		CurrentStepDistance += FVector::Distance(FirstLoc, GetActorLocation());
		FirstLoc = GetActorLocation();
		switch (CurrentMovementState)
		{
		case Walking:
			if (CurrentStepDistance >= WalkingStepDistance)
			{
				GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(WalkingCameraShake);
				UGameplayStatics::PlaySoundAtLocation(this, WalkingSound, GetActorLocation());
				CurrentStepDistance = 0;
			}
			break;
		case Running:
			if (CurrentStepDistance >= RunningStepDistance)
			{
				GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(RunningCameraShake);
				UGameplayStatics::PlaySoundAtLocation(this, WalkingSound, GetActorLocation());
				CurrentStepDistance = 0;
			}
			break;
		case Crouching:
			if (CurrentStepDistance >= CrouchingStepDistance)
			{
				GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(CrouchingCameraShake);
				UGameplayStatics::PlaySoundAtLocation(this, WalkingSound, GetActorLocation());
				CurrentStepDistance = 0;
			}
			break;
		default:
			break;
		}
	}
}

void APlatformerCharacter::CheckAllInteractables()
{
	if (CurrentOverlappedCP)
	{
		CurrentOverlappedCP->UpdateCheckPoint();
	}
}

void APlatformerCharacter::SetInsideCheckPoint(ACheckPoint* OverlappingCheckPoint)
{
	CurrentOverlappedCP = OverlappingCheckPoint;
}

void APlatformerCharacter::CreateFakeCheckPoint()
{
	if (CanCreateFakeCheckPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fake check point created. Current real check point is disabled for now"));
		FakeCheckPointLocation = GetActorLocation();
		IsFakeCheckPointActive = true;
		CanCreateFakeCheckPoint = false;
		GetWorld()->GetTimerManager().SetTimer(FakeCheckPointHandle, this, &APlatformerCharacter::ExpireFakeCheckPoint, FakeCheckPointDuration);
	}
}

void APlatformerCharacter::UseFakeCheckPoint()
{
	if (IsFakeCheckPointActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fake check point used. Current real check point is enabled"));
		SetActorLocation(FakeCheckPointLocation);
		IsFakeCheckPointActive = false;
		//CanCreateFakeCheckPoint = true;
	}
}

void APlatformerCharacter::ExpireFakeCheckPoint()
{
	if (IsFakeCheckPointActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fake check point expired. Create another one"));
		IsFakeCheckPointActive = false;
		//CanCreateFakeCheckPoint = true;
	}
}