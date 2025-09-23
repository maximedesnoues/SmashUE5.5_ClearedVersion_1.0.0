// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SmashCharacterState.h"
#include "SmashCharacterStateWalk.generated.h"

class USmashCharacterSettings;

class UAnimMontage;

/**
 * 
 */
UCLASS(ClassGroup=(SmashCharacterState), meta=(BlueprintSpawnableComponent))
class SMASHUE_API USmashCharacterStateWalk : public USmashCharacterState
{
	GENERATED_BODY()

public:
	virtual ESmashCharacterStateID GetStateID() const override;
	virtual void StateEnter(ESmashCharacterStateID PreviousStateID) override;
	virtual void StateExit(ESmashCharacterStateID NextStateID) override;
	virtual void StateTick(float DeltaTime) override;

	UFUNCTION()
	void OnInputMoveXFast(float InputMoveXFast);

	UFUNCTION()
	void OnInputJump();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Smash Character State Walk")
	float MoveSpeedMax = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Smash Character State Walk")
	TObjectPtr<UAnimMontage> WalkAnim = nullptr;

	UPROPERTY()
	const USmashCharacterSettings* CharacterSettings = nullptr;
};
