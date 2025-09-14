// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SmashCharacterState.h"
#include "Characters/SmashCharacterStateMachine.h"
#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterSettings.h"

// Sets default values for this component's properties
USmashCharacterState::USmashCharacterState()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

ESmashCharacterStateID USmashCharacterState::GetStateID() const
{
	return ESmashCharacterStateID::None;
}

void USmashCharacterState::StateInit(USmashCharacterStateMachine* InStateMachine)
{
	StateMachine = InStateMachine;
	Character = InStateMachine->GetCharacter();
	//GEngine->AddOnScreenDebugMessage(
	//	-1,
	//	3.f,
	//	FColor::Magenta,
	//	FString::Printf(TEXT("Init State %d"), GetStateID())
	//);
}

void USmashCharacterState::StateEnter(ESmashCharacterStateID PreviousStateID)
{

}

void USmashCharacterState::StateExit(ESmashCharacterStateID NextStateID)
{

}

void USmashCharacterState::StateTick(float DeltaTime)
{

}

float USmashCharacterState::GetInputMoveXThreshold() const
{
	const USmashCharacterSettings* Settings = GetDefault<USmashCharacterSettings>();
	return Settings ? Settings->InputMoveXThreshold : 0.1f;
}

bool USmashCharacterState::IsMoveXAboveThreshold() const
{
	if (Character == nullptr) return false;
	const float Threshold = GetInputMoveXThreshold();
	return FMath::Abs(Character->GetInputMoveX()) > Threshold;
}

