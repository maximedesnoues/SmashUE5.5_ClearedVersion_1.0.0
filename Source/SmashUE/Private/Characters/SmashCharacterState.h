// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SmashCharacterState.generated.h"

enum class ESmashCharacterStateID : uint8;

class ASmashCharacter;
class USmashCharacterStateMachine;

UCLASS(Abstract)
class SMASHUE_API USmashCharacterState : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USmashCharacterState();

	virtual ESmashCharacterStateID GetStateID() const;
	virtual void StateInit(USmashCharacterStateMachine* InStateMachine);
	virtual void StateEnter(ESmashCharacterStateID PreviousStateID);
	virtual void StateExit(ESmashCharacterStateID NextStateID);
	virtual void StateTick(float DeltaTime);

protected:
	UPROPERTY()
	TObjectPtr<USmashCharacterStateMachine> StateMachine = nullptr;

	UPROPERTY()
	TObjectPtr<ASmashCharacter> Character = nullptr;
};
