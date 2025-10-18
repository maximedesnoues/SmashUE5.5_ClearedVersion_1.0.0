// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CameraWorldSubsystem.generated.h"

class AActor;
class UCameraComponent;
class UWorld;

struct TStatId;

/**
 * 
 */
UCLASS()
class SMASHUE_API UCameraWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	void AddFollowTarget(AActor* FollowTarget);
	void RemoveFollowTarget(AActor* FollowTarget);

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

protected:
	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraMain = nullptr;

	UPROPERTY()
	TArray<AActor*> FollowTargets;

	UCameraComponent* FindCameraByTag(const FName& Tag) const;

	FVector CalculateAveragePositionBetweenTargets() const;

	void TickUpdateCameraPosition(float DeltaTime);
};
