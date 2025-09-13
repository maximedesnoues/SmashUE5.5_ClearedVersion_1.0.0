// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SmashCharacterStateRun.h"
#include "Characters/SmashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"

ESmashCharacterStateID USmashCharacterStateRun::GetStateID() const
{
	return ESmashCharacterStateID::Run;
}

void USmashCharacterStateRun::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Cyan,
		TEXT("Enter StateRun")
	);

	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = RunMoveSpeedMax;
	}

	PlayMontage(RunAnim, 1.f);
}

void USmashCharacterStateRun::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		TEXT("Exit StateRun")
	);

	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->StopMovementImmediately();
	}

	StopMontage(RunAnim, 0.2f);
}

void USmashCharacterStateRun::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(
		-1,
		0.1f,
		FColor::Green,
		TEXT("Tick StateRun")
	);

	const float Dir = Character->GetOrientX();
	if (FMath::Abs(Character->GetOrientX()) > KINDA_SMALL_NUMBER)
	{
		Character->AddMovementInput(FVector::ForwardVector, Dir);
	}
}

void USmashCharacterStateRun::PlayMontage(UAnimMontage* Montage, float PlayRate) const
{
	if (Character == nullptr || Montage == nullptr) return;
	if (UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance())
	{
		Anim->Montage_Play(Montage, PlayRate);
	}
}

void USmashCharacterStateRun::StopMontage(UAnimMontage* Montage, float BlendOut) const
{
	if (Character == nullptr || Montage == nullptr) return;
	if (UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance())
	{
		Anim->Montage_Stop(BlendOut, Montage);
	}
}

