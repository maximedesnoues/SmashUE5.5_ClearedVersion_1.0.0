// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/States/SmashCharacterStateRun.h"

#include "Characters/SmashCharacterStateID.h"
#include "Characters/SmashCharacterStateMachine.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterSettings.h"

#include "GameFramework/CharacterMovementComponent.h"

ESmashCharacterStateID USmashCharacterStateRun::GetStateID() const
{
	return ESmashCharacterStateID::Run;
}

void USmashCharacterStateRun::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	CharacterSettings = GetDefault<USmashCharacterSettings>();
	
	if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
	{
		Move->MaxWalkSpeed = RunMoveSpeedMax;
	}

	Character->InputJumpEvent.AddDynamic(this, &USmashCharacterStateRun::OnInputJump);

	if (RunAnim)
	{
		Character->PlayAnimMontage(RunAnim);
	}
}

void USmashCharacterStateRun::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
	{
		if (Move->IsFalling())
		{
			StateMachine->ChangeState(ESmashCharacterStateID::Fall);
			return;
		}
	}

	if (FMath::Abs(Character->GetInputMoveX()) < CharacterSettings->InputMoveXThreshold)
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

void USmashCharacterStateRun::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	Character->InputJumpEvent.RemoveDynamic(this, &USmashCharacterStateRun::OnInputJump);

	if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
	{
		Move->StopMovementImmediately();
	}
}

void USmashCharacterStateRun::OnInputJump()
{	
	if (Character->CanJump())
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Jump);
		return;
	}
}
