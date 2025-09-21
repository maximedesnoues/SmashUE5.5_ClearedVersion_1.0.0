// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SmashCharacterStateMachine.generated.h"

enum class ESmashCharacterStateID : uint8;

class ASmashCharacter;
class USmashCharacterState;

/**
 * 
 */
UCLASS()
class SMASHUE_API USmashCharacterStateMachine : public UObject
{
	GENERATED_BODY()

public:
	void Init(ASmashCharacter* InCharacter);
	void Tick(float DeltaTime);

	ASmashCharacter* GetCharacter() const;

	USmashCharacterState* GetState(ESmashCharacterStateID StateID) const;

	UFUNCTION(BlueprintCallable)
	void ChangeState(ESmashCharacterStateID NextStateID);

protected:
	UPROPERTY()
	TObjectPtr<ASmashCharacter> Character = nullptr;

	UPROPERTY()
	TArray<USmashCharacterState*> AllStates;

	UPROPERTY(BlueprintReadOnly)
	ESmashCharacterStateID CurrentStateID;

	UPROPERTY()
	TObjectPtr<USmashCharacterState> CurrentState = nullptr;

	void FindStates();
	void InitStates();
};
