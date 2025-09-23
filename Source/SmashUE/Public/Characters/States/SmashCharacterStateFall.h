// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SmashCharacterState.h"
#include "SmashCharacterStateFall.generated.h"

class USmashCharacterSettings;

class UAnimMontage;

/**
 *
 */
UCLASS(ClassGroup=(SmashCharacterState), meta=(BlueprintSpawnableComponent))
class SMASHUE_API USmashCharacterStateFall : public USmashCharacterState
{
    GENERATED_BODY()

public:
    virtual ESmashCharacterStateID GetStateID() const override;
    virtual void StateEnter(ESmashCharacterStateID PreviousStateID) override;
    virtual void StateExit(ESmashCharacterStateID NextStateID) override;
    virtual void StateTick(float DeltaTime) override;

    UFUNCTION()
    void OnInputFallFast(float InputFallFast);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fall")
    float FallGravityScale = 3.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fall")
    float FallFastGravityScale = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fall")
    float FallHorizontalMoveSpeed = 400.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fall")
    float FallAirControl = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fall")
    TObjectPtr<UAnimMontage> FallAnim = nullptr;

    UPROPERTY()
    const USmashCharacterSettings* CharacterSettings = nullptr;
};
