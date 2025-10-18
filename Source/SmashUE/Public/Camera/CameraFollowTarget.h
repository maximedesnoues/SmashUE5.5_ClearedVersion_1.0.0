// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CameraFollowTarget.generated.h"

UINTERFACE()
class SMASHUE_API UCameraFollowTarget : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class SMASHUE_API ICameraFollowTarget
{
	GENERATED_BODY()

public:
	virtual bool IsFollowable() = 0;
	virtual FVector GetFollowPosition() = 0;
};
