// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Characters/SmashCharacterInputData.h"
#include "InputMappingContext.h"
#include "SmashCharacterSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Smash Character Settings"))
class SMASHUE_API USmashCharacterSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
    UPROPERTY(Config, EditAnywhere, Category="Inputs")
    TSoftObjectPtr<USmashCharacterInputData> InputData;

    UPROPERTY(Config, EditAnywhere, Category="Inputs")
    TSoftObjectPtr<UInputMappingContext> InputMappingContext;

    UPROPERTY(Config, EditAnywhere, Category="Inputs", meta=(DisplayName="Input Move X Threshold", ClampMin="0.0", UIMin="0.0"))
    float InputMoveXThreshold = 0.1f;
	
};
