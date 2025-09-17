// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SmashCharacterStateFall.h"
#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterSettings.h"
#include "Characters/SmashCharacterStateMachine.h"
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
        Move->MaxWalkSpeed = FallHorizontalMoveSpeed;
        Move->AirControl = FallAirControl;
        Move->GravityScale = FallGravityScale;
    }
    
    Character->PlayAnimMontage(FallAnim);

    Character->InputMoveYFastEvent.AddDynamic(this, &USmashCharacterStateFall::OnInputMoveYFast);
}

void USmashCharacterStateFall::StateExit(ESmashCharacterStateID NextStateID)
{
    Super::StateExit(NextStateID);

    if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
    {
        Move->GravityScale = 1.f;
    }

    Character->InputMoveYFastEvent.RemoveDynamic(this, &USmashCharacterStateFall::OnInputMoveYFast);
}

void USmashCharacterStateFall::StateTick(float DeltaTime)
{
    Super::StateTick(DeltaTime);

    if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
    {
        if (Move->IsMovingOnGround())
        {
            StateMachine->ChangeState(ESmashCharacterStateID::Idle);
        }
    }

    if (FMath::Abs(Character->GetInputMoveX()) > CharacterSettings->InputMoveXThreshold)
    {
        Character->SetOrientX(Character->GetInputMoveX());
        Character->AddMovementInput(FVector::ForwardVector, Character->GetOrientX());
    }
}

void USmashCharacterStateFall::OnInputMoveYFast(float InputMoveY)
{
    if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
    {
        if (InputMoveY < -0.5f)
        {
            Move->GravityScale = FallFastGravityScale;
        }
    }
}

