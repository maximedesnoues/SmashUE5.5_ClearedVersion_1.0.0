// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SmashCharacterStateJump.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterSettings.h"
#include "Characters/SmashCharacterStateID.h"
#include "Characters/SmashCharacterStateMachine.h"

#include "GameFramework/CharacterMovementComponent.h"

ESmashCharacterStateID USmashCharacterStateJump::GetStateID() const
{
    return ESmashCharacterStateID::Jump;
}

void USmashCharacterStateJump::StateEnter(ESmashCharacterStateID PreviousStateID)
{
    Super::StateEnter(PreviousStateID);

    CharacterSettings = GetDefault<USmashCharacterSettings>();
    
	if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
	{
		SavedGravityScale = Move->GravityScale;
		SavedJumpZVelocity = Move->JumpZVelocity;
		SavedMaxWalkSpeed = Move->MaxWalkSpeed;
		SavedAirControl = Move->AirControl;

		const float h = FMath::Max(0.f, JumpMaxHeight);
		const float th = FMath::Max(KINDA_SMALL_NUMBER, JumpDuration);

		const float RequiredGravityZ = -2.f * h / FMath::Square(th);
		const float RequiredInitialVerticalVelocity = (2.f * h) / th;

		const float CurrentGravityZWithScale = Move->GetGravityZ();
		const float WorldGravityZ = FMath::IsNearlyZero(SavedGravityScale) ? CurrentGravityZWithScale : CurrentGravityZWithScale / SavedGravityScale;

		float NewGravityScale = SavedGravityScale;
		if (!FMath::IsNearlyZero(WorldGravityZ))
		{
			NewGravityScale = RequiredGravityZ / WorldGravityZ;
		}

		Move->GravityScale = NewGravityScale;
		Move->JumpZVelocity = RequiredInitialVerticalVelocity;
		Move->MaxWalkSpeed = JumpWalkSpeed;
		Move->AirControl = JumpAirControl;
	}

	Character->Jump();

	if (JumpAnim)
	{
		Character->PlayAnimMontage(JumpAnim);
	}
}

void USmashCharacterStateJump::StateExit(ESmashCharacterStateID NextStateID)
{
    Super::StateExit(NextStateID);

	if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
	{
		Move->GravityScale = SavedGravityScale;
		Move->JumpZVelocity = SavedJumpZVelocity;
		Move->MaxWalkSpeed = SavedMaxWalkSpeed;
		Move->AirControl = SavedAirControl;
	}
}

void USmashCharacterStateJump::StateTick(float DeltaTime)
{
    Super::StateTick(DeltaTime);

    if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
    {
        if (Move->IsFalling() && Character->GetVelocity().Z <= 0.f)
        {
            StateMachine->ChangeState(ESmashCharacterStateID::Fall);
            return;
        }
    }

    if (FMath::Abs(Character->GetInputMoveX()) > CharacterSettings->InputMoveXThreshold)
    {
        Character->SetOrientX(Character->GetInputMoveX());
        Character->AddMovementInput(FVector::ForwardVector, Character->GetOrientX());
    }
}

