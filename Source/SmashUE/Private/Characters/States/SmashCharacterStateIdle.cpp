// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SmashCharacterStateIdle.h"
#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterStateMachine.h"
#include "Animation/AnimInstance.h"

ESmashCharacterStateID USmashCharacterStateIdle::GetStateID() const
{
	return ESmashCharacterStateID::Idle;
}

void USmashCharacterStateIdle::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Cyan,
		TEXT("Enter StateIdle")
	);

	PlayMontage(IdleAnim, 1.f);
}

void USmashCharacterStateIdle::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		TEXT("Exit StateIdle")
	);

	StopMontage(IdleAnim, 0.2f);
}

void USmashCharacterStateIdle::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(
		-1,
		0.1f,
		FColor::Green,
		TEXT("Tick StateIdle")
	);

	if (FMath::Abs(Character->GetInputMoveX()) > 0.1f)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Walk);
	}
}

void USmashCharacterStateIdle::PlayMontage(UAnimMontage* Montage, float PlayRate) const
{
	if (Character == nullptr || Montage == nullptr) return;
	if (UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance())
	{
		Anim->Montage_Play(Montage, PlayRate);
	}
}

void USmashCharacterStateIdle::StopMontage(UAnimMontage* Montage, float BlendOut) const
{
	if (Character == nullptr || Montage == nullptr) return;
	if (UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance())
	{
		Anim->Montage_Stop(BlendOut, Montage);
	}
}

