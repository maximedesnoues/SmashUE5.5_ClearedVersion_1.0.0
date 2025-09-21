// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SmashCharacterInputData.generated.h"

class UInputAction;

/**
 * 
 */
UCLASS()
class SMASHUE_API USmashCharacterInputData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta=(DisplayName="Input Action Move X"))
	TObjectPtr<UInputAction> InputActionMoveX;

	UPROPERTY(EditAnywhere, meta=(DisplayName="Input Action Move X Fast"))
	TObjectPtr<UInputAction> InputActionMoveXFast;

	UPROPERTY(EditAnywhere, meta=(DisplayName="Input Action Jump"))
	TObjectPtr<UInputAction> InputActionJump;

	UPROPERTY(EditAnywhere, meta=(DisplayName="Input Action Fall Fast"))
	TObjectPtr<UInputAction> InputActionFallFast;
};
