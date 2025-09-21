// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/States/SmashCharacterStateIdle.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterSettings.h"
#include "Characters/SmashCharacterStateID.h"
#include "Characters/SmashCharacterStateMachine.h"

#include "GameFramework/CharacterMovementComponent.h"

ESmashCharacterStateID USmashCharacterStateIdle::GetStateID() const
{
	return ESmashCharacterStateID::Idle;
}

void USmashCharacterStateIdle::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	if (!Character)
	{
		return;
	}

	CharacterSettings = GetDefault<USmashCharacterSettings>();

	Character->InputMoveXFastEvent.AddDynamic(this, &USmashCharacterStateIdle::OnInputMoveXFast);
	Character->InputJumpEvent.AddDynamic(this, &USmashCharacterStateIdle::OnInputJump);

	if (IdleAnim)
	{
		Character->PlayAnimMontage(IdleAnim);
	}
}

void USmashCharacterStateIdle::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	if (!Character)
	{
		return;
	}
	
	Character->InputMoveXFastEvent.RemoveDynamic(this, &USmashCharacterStateIdle::OnInputMoveXFast);
	Character->InputJumpEvent.RemoveDynamic(this, &USmashCharacterStateIdle::OnInputJump);
}

void USmashCharacterStateIdle::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	if (!Character)
	{
		return;
	}

	if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
	{
		if (Move->IsFalling())
		{
			StateMachine->ChangeState(ESmashCharacterStateID::Fall);
			return;
		}
	}

	if (CharacterSettings && FMath::Abs(Character->GetInputMoveX()) > CharacterSettings->InputMoveXThreshold)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Walk);
		return;
	}
}

void USmashCharacterStateIdle::OnInputMoveXFast(float InputMoveXFast)
{
	if (!StateMachine)
	{
		return;
	}
	
	StateMachine->ChangeState(ESmashCharacterStateID::Run);
}

void USmashCharacterStateIdle::OnInputJump()
{
	if (!StateMachine)
	{
		return;
	}
	
	if (!Character->CanJump())
	{
		return;
	}

	StateMachine->ChangeState(ESmashCharacterStateID::Jump);
}

