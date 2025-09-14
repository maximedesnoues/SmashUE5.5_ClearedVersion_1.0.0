// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SmashCharacterState.h"
#include "SmashCharacterStateIdle.generated.h"

class UAnimMontage;
class USmashCharacterSettings;

/**
 * 
 */
UCLASS(ClassGroup=(SmashCharacterState), meta=(BlueprintSpawnableComponent))
class SMASHUE_API USmashCharacterStateIdle : public USmashCharacterState
{
	GENERATED_BODY()

public:
	virtual ESmashCharacterStateID GetStateID() const override;

	virtual void StateEnter(ESmashCharacterStateID PreviousStateID) override;

	virtual void StateExit(ESmashCharacterStateID NextStateID) override;

	virtual void StateTick(float DeltaTime) override;

	UFUNCTION()
	void OnInputMoveXFast(float InputMoveX);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Smash Character State Idle")
	TObjectPtr<UAnimMontage> IdleAnim;

	UPROPERTY()
	const USmashCharacterSettings* CharacterSettings;
	
};
