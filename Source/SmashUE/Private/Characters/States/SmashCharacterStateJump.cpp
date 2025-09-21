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

    if (!Character)
    {
        return;
    }

    CharacterSettings = GetDefault<USmashCharacterSettings>();
    
    if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
    {
        SavedJumpZVelocity = Move->JumpZVelocity;

        const float GravityZ = Move->GetGravityZ();
        const float g = FMath::Abs(GravityZ);
        const float TargetV = FMath::Sqrt(2.f * g * JumpMaxHeight);

        Move->JumpZVelocity = TargetV;
        Move->MaxWalkSpeed = JumpWalkSpeed;
        Move->AirControl = JumpAirControl;
    }
    
    Character->Jump();

    if (JumpDuration > 0.f)
    {
        Character->GetWorldTimerManager().SetTimer(JumpTimerHandle, this, &ThisClass::StopJump, JumpDuration, false);
    }

    if (JumpAnim)
    {
        Character->PlayAnimMontage(JumpAnim);
    }
}

void USmashCharacterStateJump::StateExit(ESmashCharacterStateID NextStateID)
{
    Super::StateExit(NextStateID);

    if (!Character)
    {
        return;
    }

    Character->GetWorldTimerManager().ClearTimer(JumpTimerHandle);

    if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
    {
        Move->JumpZVelocity = SavedJumpZVelocity;
    }
}

void USmashCharacterStateJump::StateTick(float DeltaTime)
{
    Super::StateTick(DeltaTime);

    if (!Character)
    {
        return;
    }

    if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
    {
        if (Move->IsFalling() && Character->GetVelocity().Z <= 0.f)
        {
            StateMachine->ChangeState(ESmashCharacterStateID::Fall);
            return;
        }
    }

    if (CharacterSettings && FMath::Abs(Character->GetInputMoveX()) > CharacterSettings->InputMoveXThreshold)
    {
        Character->SetOrientX(Character->GetInputMoveX());
        Character->AddMovementInput(FVector::ForwardVector, Character->GetOrientX());
    }
}

void USmashCharacterStateJump::StopJump()
{
    if (!Character)
    {
        return;
    }

    if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
    {
        FVector Velocity = Character->GetVelocity();
        
        if (Velocity.Z > 0.f)
        {
            Velocity.Z = 0.f;
            Move->Velocity = Velocity;
        }
    }
}

