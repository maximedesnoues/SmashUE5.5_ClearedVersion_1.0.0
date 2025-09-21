// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SmashCharacterState.h"
#include "SmashCharacterStateJump.generated.h"

class USmashCharacterSettings;

class UAnimMontage;

/**
 *
 */
UCLASS(ClassGroup=(SmashCharacterState), meta=(BlueprintSpawnableComponent))
class SMASHUE_API USmashCharacterStateJump : public USmashCharacterState
{
	GENERATED_BODY()

public:
	virtual ESmashCharacterStateID GetStateID() const override;
	virtual void StateEnter(ESmashCharacterStateID PreviousStateID) override;
	virtual void StateExit(ESmashCharacterStateID NextStateID) override;
	virtual void StateTick(float DeltaTime) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Jump")
    float JumpMaxHeight = 280.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Jump")
    float JumpDuration = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Jump")
    float JumpWalkSpeed = 400.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Jump")
    float JumpAirControl = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Jump")
    TObjectPtr<UAnimMontage> JumpAnim;

    UPROPERTY()
    const USmashCharacterSettings* CharacterSettings = nullptr;

private:
    FTimerHandle JumpTimerHandle;
    float SavedJumpZVelocity = 0.f;

    void StopJump();
};
