// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CameraSettings.generated.h"

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Camera Settings"))
class SMASHUE_API UCameraSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category="Tags")
	FName CameraMainTag = TEXT("CameraMain");

	UPROPERTY(Config, EditAnywhere, Category="Tags")
	FName CameraBoundsTag = TEXT("CameraBounds");

	UPROPERTY(Config, EditAnywhere, Category="Tags")
	FName CameraDistanceMinTag = TEXT("CameraDistanceMin");

	UPROPERTY(Config, EditAnywhere, Category="Tags")
	FName CameraDistanceMaxTag = TEXT("CameraDistanceMax");

	UPROPERTY(Config, EditAnywhere, Category="Zoom")
	float DistanceBetweenTargetsMin = 300.f;

	UPROPERTY(Config, EditAnywhere, Category="Zoom")
	float DistanceBetweenTargetsMax = 1500.f;

	UPROPERTY(Config, EditAnywhere, Category="Damping")
	float PositionDampingFactor = 5.f;

	UPROPERTY(Config, EditAnywhere, Category="Damping")
	float SizeDampingFactor = 5.f;
};
