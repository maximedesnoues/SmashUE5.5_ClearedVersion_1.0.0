// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "ArenaPlayerStartOrient.h"
#include "ArenaPlayerStart.generated.h"

UCLASS()
class SMASHUE_API AArenaPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EArenaPlayerStartOrient StartOrient = EArenaPlayerStartOrient::Right;

	UFUNCTION()
	float GetStartOrientX() const;
};
