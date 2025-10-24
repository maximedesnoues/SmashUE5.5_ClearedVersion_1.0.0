// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/States/SmashCharacterStateFall.h"

#include "Characters/SmashCharacterStateID.h"
#include "Characters/SmashCharacterStateMachine.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterSettings.h"

#include "GameFramework/CharacterMovementComponent.h"

ESmashCharacterStateID USmashCharacterStateFall::GetStateID() const
{
    return ESmashCharacterStateID::Fall;
}

void USmashCharacterStateFall::StateEnter(ESmashCharacterStateID PreviousStateID)
{
    Super::StateEnter(PreviousStateID);

    CharacterSettings = GetDefault<USmashCharacterSettings>();

    if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
    {
        Move->GravityScale = FallGravityScale;
        Move->MaxWalkSpeed = FallHorizontalMoveSpeed;
        Move->AirControl = FallAirControl;
    }

    Character->InputFallFastEvent.AddDynamic(this, &USmashCharacterStateFall::OnInputFallFast);

    if (FallAnim)
    {
        Character->PlayAnimMontage(FallAnim);
    }
}

void USmashCharacterStateFall::StateTick(float DeltaTime)
{
    Super::StateTick(DeltaTime);

    if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
    {
        if (Move->IsMovingOnGround())
        {
            StateMachine->ChangeState(ESmashCharacterStateID::Idle);
            return;
        }
    }

    if (FMath::Abs(Character->GetInputMoveX()) > CharacterSettings->InputMoveXThreshold)
    {
        Character->SetOrientX(Character->GetInputMoveX());
        Character->AddMovementInput(FVector::ForwardVector, Character->GetOrientX());
    }
}

void USmashCharacterStateFall::StateExit(ESmashCharacterStateID NextStateID)
{
    Super::StateExit(NextStateID);

    Character->InputFallFastEvent.RemoveDynamic(this, &USmashCharacterStateFall::OnInputFallFast);

    if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
    {
        Move->GravityScale = 1.f;
    }
}

void USmashCharacterStateFall::OnInputFallFast(float InputFallFast)
{
    if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
    {
        if (InputFallFast < -0.5f)
        {
            Move->GravityScale = FallFastGravityScale;
        }
    }
}
