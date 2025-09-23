// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SmashCharacterStateMachine.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterState.h"
#include "Characters/SmashCharacterStateID.h"

void USmashCharacterStateMachine::Init(ASmashCharacter* InCharacter)
{
    Character = InCharacter;
    FindStates();
    InitStates();
    ChangeState(ESmashCharacterStateID::Idle);
}

void USmashCharacterStateMachine::FindStates()
{
    TArray<UActorComponent*> FoundComponents = Character->K2_GetComponentsByClass(USmashCharacterState::StaticClass());
    for (UActorComponent* StateComponent : FoundComponents)
    {
        if (USmashCharacterState* State = Cast<USmashCharacterState>(StateComponent))
        {
            if (State->GetStateID() != ESmashCharacterStateID::None)
            {
                AllStates.Add(State);
            }
        }
    }
}

void USmashCharacterStateMachine::InitStates()
{
    for (USmashCharacterState* State : AllStates)
    {
        State->StateInit(this);
    }
}

ASmashCharacter* USmashCharacterStateMachine::GetCharacter() const
{
    return Character;
}

USmashCharacterState* USmashCharacterStateMachine::GetState(ESmashCharacterStateID StateID) const
{
    for (USmashCharacterState* State : AllStates)
    {
        if (State->GetStateID() == StateID)
        {
            return State;
        }
    }
    
    return nullptr;
}

void USmashCharacterStateMachine::ChangeState(ESmashCharacterStateID NextStateID)
{
    if (USmashCharacterState* NextState = GetState(NextStateID))
    {
        if (CurrentState)
        {
            CurrentState->StateExit(NextStateID);
        }

        const ESmashCharacterStateID PreviousStateID = CurrentStateID;
        CurrentStateID = NextStateID;
        CurrentState = NextState;

        if (CurrentState)
        {
            CurrentState->StateEnter(PreviousStateID);
        }
    }
}

void USmashCharacterStateMachine::Tick(float DeltaTime)
{
    if (!CurrentState)
    {
        return;
    }

    CurrentState->StateTick(DeltaTime);
}

