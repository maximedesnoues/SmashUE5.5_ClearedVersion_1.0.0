// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CameraWorldSubsystem.generated.h"

class UCameraComponent;

/**
 * 
 */
UCLASS()
class SMASHUE_API UCameraWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

#pragma region Subsystem Overrides

public:
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

#pragma endregion

#pragma region Main Camera

protected:
	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraMain = nullptr;

	UCameraComponent* FindCameraByTag(const FName& Tag) const;

	FVector CalculateAveragePositionBetweenTargets() const;

	void TickUpdateCameraPosition(float DeltaTime);

#pragma endregion

#pragma region Follow Targets

public:
	void AddFollowTarget(UObject* FollowTarget);
	void RemoveFollowTarget(UObject* FollowTarget);

protected:
	UPROPERTY()
	TArray<UObject*> FollowTargets;

#pragma endregion

#pragma region Bounds

protected:
	UPROPERTY()
	FVector2D CameraBoundsMin = FVector2D::ZeroVector;

	UPROPERTY()
	FVector2D CameraBoundsMax = FVector2D::ZeroVector;

	UPROPERTY()
	float CameraBoundsYProjectionCenter = 0.f;

	AActor* FindCameraBoundsActor();
	void InitCameraBounds(AActor* CameraBoundsActor);

	void GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax);
	FVector CalculateWorldPositionFromViewportPosition(const FVector2D& ViewportPosition);

	void ClampPositionIntoCameraBounds(FVector& Position);

#pragma endregion
};
