// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SmashCharacterState.h"
#include "SmashCharacterStateRun.generated.h"

class USmashCharacterSettings;

class UAnimMontage;

/**
 *
 */
UCLASS(ClassGroup=(SmashCharacterState), meta=(BlueprintSpawnableComponent))
class SMASHUE_API USmashCharacterStateRun : public USmashCharacterState
{
	GENERATED_BODY()

public:
	virtual ESmashCharacterStateID GetStateID() const override;
	virtual void StateEnter(ESmashCharacterStateID PreviousStateID) override;
	virtual void StateExit(ESmashCharacterStateID NextStateID) override;
	virtual void StateTick(float DeltaTime) override;

	UFUNCTION()
	void OnInputJump();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Smash Character State Run")
	float RunMoveSpeedMax = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Smash Character State Run")
	TObjectPtr<UAnimMontage> RunAnim = nullptr;

	UPROPERTY()
	const USmashCharacterSettings* CharacterSettings = nullptr;
};
