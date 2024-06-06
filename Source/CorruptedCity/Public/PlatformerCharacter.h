// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlatformerCharacter.generated.h"


enum WallRunSide
{
	Right = 0,
	Left = 1
};

enum EndWallRunReason
{
	FallOfWall = 0,
	JumpOfWall = 1,
	LandOnSurface = 2
};
enum MovementState
{
	Walking = 0,
	Running = 1,
	Crouching = 2,
	Sliding = 3
};

UCLASS()
class CORRUPTEDCITY_API APlatformerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlatformerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Landed(const FHitResult& Hit);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION(BlueprintCallable)
		void CameraTiltEffectValue(float CameraParam);
	UFUNCTION(BlueprintImplementableEvent)
		void StartCameraTilt();
	UFUNCTION(BlueprintImplementableEvent)
		void EndCameraTilt();
	UFUNCTION(BlueprintImplementableEvent)
		void BeginCrouchEffect();
	UFUNCTION(BlueprintImplementableEvent)
		void EndCrouchEffect();


	UFUNCTION(BlueprintImplementableEvent)
		void BeginSlideCameraTilt();
	UFUNCTION(BlueprintImplementableEvent)
		void EndSlideCameraTilt();

	UFUNCTION(BlueprintImplementableEvent)
		void StartDashingCameraEffect();
	UFUNCTION(BlueprintImplementableEvent)
		void EndDashingCameraEffect();

	UFUNCTION(BlueprintImplementableEvent)
		void GrappleRopeSystem(bool Grappling);

	UCharacterMovementComponent* GetPCharacterMovementComponent() { return MovementComponent; }
private:
	UPROPERTY(VisibleAnywhere, Category = "Camera System")
		class UCameraComponent* FirstPersonCamera;
	UPROPERTY(EditAnywhere, Category = "Preferences")
		// Mouse sensivity parameter
		float MouseRotationSensitivity = 10.f;
	UPROPERTY(VisibleAnywhere, Category = "Stealth")
		class UStealthHealthComponent* PlayerStealthHealth;

	float DeltaSeconds;
	AController* PlayerController;
	class UGrabber* InteractHandler;
	class UPhysicsHandleComponent* PhysicsHandler;
	UCharacterMovementComponent* MovementComponent;

	float ForwardAxis;
	float RightAxis;
	float MainGravityScale;
	float MainFOVScale;


	void CharacterHorizontalRotation(float Value);
	void CharacterVerticalRotation(float Value);
	void CharacterLocalForwardMove(float Value);
	void CharacterLocalRightMove(float Value);
	void JumpingSystem();
	void StopJump();
	void GrabObject_Special();
	void ReleaseObject_Special();
	void StartRunning();
	void EndRunning();
	void StartCrouching();
	void EndCrouching();


	FVector FindLauchVelocity() const;
	void CalculateRealHorizontalVelocity();
	int32 FrameNum;
	FVector2D ActorLocation1;
	FVector2D ActorLocation2;
	float RealHorizontalVelocity;

	// WallRunning
	UPROPERTY(EditAnywhere, Category = "WallRunning Adjustments")
		// Min speed to trigger wall running system. Under this speed can't execute wall running
		float MinWallRunStartSpeed = 300.f;
	UPROPERTY(EditAnywhere, Category = "WallRunning Adjustments")
		// Min speed to continue wall running. Under this speed will result with falling from wall
		float MinWallRunContinueSpeed = 200.f;
	UPROPERTY(EditAnywhere, Category = "WallRunning Adjustments")
		// Wall running falling gravity. Must be too lower than original gravity scale
		float WallRunGravity = 0.09f;

	bool IsWallRunning;
	bool CanWallRun = true;

	FName CantWallRunnableTags = "CannotWallRun";
	FVector WallRunDirection;
	WallRunSide CurrentWallRunSide;
	WallRunSide LastSide;
	EndWallRunReason EndReason;
	FHitResult WallRunHitResult;

	UFUNCTION(BlueprintCallable)
		void CheckWallRunConditions(const FVector& HitImpactNormal, UPrimitiveComponent* OtherComp);
	bool CanSurfaceBeWallRun(const FVector& SurfaceNormal, UPrimitiveComponent* WallComponent);
	bool AreRequiredKeysDown();
	FVector FindWallRunSideAndDirection(const FVector& WallNormal, WallRunSide& out_LastSetSide);
	void BeginWallRun();
	void UpdateWallRun();
	void EndWallRun(const EndWallRunReason& EndRunReason);



	//Wall Running End
	// 
	// Jumping System
	int32 CurrentJumpCount = 0;
	UPROPERTY(EditAnywhere, Category = "Movement Adjustments")
		int32 MaxJumpCount = 2;
	// Jumping System End
	// 
	// Movement System
	bool SprintKeyPressed;
	bool CrouchKeyPressed;
	bool IsSliding;
	bool ShouldSlide;
public:
	float StandingCapsuleHalfHeight;
private:
	float StandingCameraZOffset;
	MovementState CurrentMovementState;
	MovementState PrevMovementState;
	FHitResult CrouchingHitResult;
	void ChangeMovementState(int32 MovementStateIndex);
	void OnMovementStateChange(int32 PrevMovementStateIndex);
	void ResolveMovementState();
	bool CanSprint();
	bool CanStand();
	void BeginSlide();
	void EndSlide();
	void SlideUpdate();
	UFUNCTION(BlueprintCallable)
		void SlideCameraTiltUpdate(float TiltValue);

	FVector CalculateFloorInfluence(const FVector& FloorNormal);
	UFUNCTION(BlueprintCallable)
		void UpdateCrouchSystem(float HalfHeightParam);
	UPROPERTY(EditAnywhere, Category = "Movement Adjustments")
		// Max walking speed
		float WalkingSpeed = 380.f;
	UPROPERTY(EditAnywhere, Category = "Movement Adjustments")
		// Max running speed
		float RunningSpeed = 700.f;
	UPROPERTY(EditAnywhere, Category = "Movement Adjustments")
		// Max crouching speed
		float CrouchingSpeed = 220.f;
	UPROPERTY(EditAnywhere, Category = "Camera Shakes")
		TSubclassOf<class UCameraShakeBase> WalkingCameraShake;
	UPROPERTY(EditAnywhere, Category = "Camera Shakes")
		TSubclassOf<class UCameraShakeBase> RunningCameraShake;
	UPROPERTY(EditAnywhere, Category = "Camera Shakes")
		TSubclassOf<class UCameraShakeBase> CrouchingCameraShake;
	UPROPERTY(EditAnywhere, Category = "Sound Effects")
		class USoundBase* WalkingSound;

	void WalkingCalculations();
	float CurrentStepDistance;
	UPROPERTY(EditAnywhere, Category = "Movement Adjustments")
		// Distance between steps while the player walking
		float WalkingStepDistance = 150;
	UPROPERTY(EditAnywhere, Category = "Movement Adjustments")
		// Distance between steps while the player running
		float RunningStepDistance = 190;
	UPROPERTY(EditAnywhere, Category = "Movement Adjustments")
		// Distance between steps while the player crouching
		float CrouchingStepDistance = 100;
	int32 CurrentFrame;
	FVector FirstLoc;
	FVector ForwardVelocity;
	// Movement System End
	// 
	// Climbing System
	UPROPERTY(EditAnywhere, Category = "Movement Adjustments")
		// Constant climbing speed
		float ClimbingSpeed = 200.f;
	void AutoClimbingSystem();
	void ClimbUpdate();
	void HoldEdge();
	void ReleaseHoldEdge();
	FHitResult AutoClimbingResult_1;
	FHitResult AutoClimbingResult_2;
	FHitResult AutoClimbingResult_3;
	bool IsHanging;
	bool CanHoldEdge;
	UStaticMeshComponent* PrevLedgeComponent;
	FVector TargetLocation;
	FVector HangingLocation;
	FVector PossibleMovableDirection;
	FVector DirectionToLedge;

	float DistanceBetweenLedges;
	UPROPERTY(EditAnywhere, Category = "Movement Adjustments")
		// Max height player can climb. Heigher ledges can't trigger climbing system
		float MaxClimbZDistance = 100;

	// Climbing System End
	// 
	// Dashing System
	UPROPERTY(EditAnywhere, Category = "Movement Adjustments")
		float DashingForceParameter = 1500.f;
	UPROPERTY(EditAnywhere, Category = "Movement Adjustments")
		float MaxDashWaitTime = 1.5f;
	UPROPERTY(EditAnywhere, Category = "Skill System")
		bool CanDash = true;
	bool TryingToDash;
	bool HasDashed;
	FTimerHandle DashTimerHandle;
	void StartDashing();
	UFUNCTION(BlueprintCallable)
		void UpdateDashFOV(float FOV_Value);
	void ExecuteDashing();
	// Dashing System End
	// 
	// Grapple System
	UPROPERTY(EditAnywhere, Category = "Movement Adjustments")
		float GrappleMaxDistance = 1500;
	UPROPERTY(EditAnywhere, Category = "Movement Adjustments")
		float GrappleForce = 1.75f;
	UPROPERTY(EditAnywhere, Category = "Movement Adjustments")
		float FlyingSpeed = 1750.f;
	bool IsGrappling;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		// The position where cable will connect
		FVector GrappleTargetLocation;
	void CheckGrappleConditions();
	void ExecuteGrappleSystem();
	void FinishGrappling();
	FHitResult GrappleHitResult;

	// Grapple System End

	void CheckAllInteractables();
public:
	void SetInsideCheckPoint(class ACheckPoint* OverlappingCheckPoint);
	ACheckPoint* CurrentOverlappedCP;


	// Skills (Gainable Slots)
		// Skill 1
private:
	void CreateFakeCheckPoint();
	FVector FakeCheckPointLocation;
	FTimerHandle FakeCheckPointHandle;
public:
	void UseFakeCheckPoint();
	bool IsFakeCheckPointActive;
	bool CanCreateFakeCheckPoint = true;
private:
	void ExpireFakeCheckPoint();
	float FakeCheckPointDuration = 10.f;
	
};
