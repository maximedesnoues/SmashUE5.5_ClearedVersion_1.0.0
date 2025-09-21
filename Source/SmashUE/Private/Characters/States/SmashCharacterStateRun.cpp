// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/States/SmashCharacterStateRun.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterSettings.h"
#include "Characters/SmashCharacterStateID.h"
#include "Characters/SmashCharacterStateMachine.h"

#include "GameFramework/CharacterMovementComponent.h"

ESmashCharacterStateID USmashCharacterStateRun::GetStateID() const
{
	return ESmashCharacterStateID::Run;
}

void USmashCharacterStateRun::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	if (!Character)
	{
		return;
	}

	CharacterSettings = GetDefault<USmashCharacterSettings>();
	
	Character->InputJumpEvent.AddDynamic(this, &USmashCharacterStateRun::OnInputJump);

	if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
	{
		Move->MaxWalkSpeed = RunMoveSpeedMax;
	}

	if (RunAnim)
	{
		Character->PlayAnimMontage(RunAnim);
	}
}

void USmashCharacterStateRun::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	if (!Character)
	{
		return;
	}
	
	Character->InputJumpEvent.RemoveDynamic(this, &USmashCharacterStateRun::OnInputJump);

	if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
	{
		Move->StopMovementImmediately();
	}
}

void USmashCharacterStateRun::StateTick(float DeltaTime)
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

	if (CharacterSettings && FMath::Abs(Character->GetInputMoveX()) < CharacterSettings->InputMoveXThreshold)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Idle);
		return;
	}
	else
	{
		Character->SetOrientX(Character->GetInputMoveX());
		Character->AddMovementInput(FVector::ForwardVector, Character->GetOrientX());
	}
}

void USmashCharacterStateRun::OnInputJump()
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

