// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlatformerGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CORRUPTEDCITY_API APlatformerGameMode : public AGameModeBase
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public:
	void SetNewCheckPoint(class ACheckPoint* NewCheckPoint);
	void ReturnToLastCheckPoint();

private:
	class ACheckPoint* ActiveCheckPoint;
	TArray<ACheckPoint*> AllUnlockedCheckPoints;

	class APlatformerCharacter* PCharacter;
};
