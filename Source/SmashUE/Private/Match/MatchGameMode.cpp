// Fill out your copyright notice in the Description page of Project Settings.

#include "Match/MatchGameMode.h"

#include "Arena/ArenaPlayerStart.h"
#include "Arena/ArenaSettings.h"
#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterInputData.h"
#include "Characters/SmashCharacterSettings.h"

#include "Kismet/GameplayStatics.h"

void AMatchGameMode::BeginPlay()
{
    Super::BeginPlay();

    TArray<AArenaPlayerStart*> PlayerStartsPoints;
    FindPlayerStartActorsInArena(PlayerStartsPoints);
    SpawnCharacters(PlayerStartsPoints);
}

USmashCharacterInputData* AMatchGameMode::LoadInputDataFromConfig() const
{
    const USmashCharacterSettings* CharacterSettings = GetDefault<USmashCharacterSettings>();
    if (!CharacterSettings)
    {
        return nullptr;
    }
    
    return CharacterSettings->InputData.LoadSynchronous();
}

UInputMappingContext* AMatchGameMode::LoadInputMappingContextFromConfig() const
{
    const USmashCharacterSettings* CharacterSettings = GetDefault<USmashCharacterSettings>();
    if (!CharacterSettings)
    {
        return nullptr;
    }
    
    return CharacterSettings->InputMappingContext.LoadSynchronous();
}

void AMatchGameMode::FindPlayerStartActorsInArena(TArray<AArenaPlayerStart*>& ResultsActors) const
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AArenaPlayerStart::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        if (AArenaPlayerStart* ArenaPlayerStartActor = Cast<AArenaPlayerStart>(Actor))
        {
            ResultsActors.Add(ArenaPlayerStartActor);
        }
    }
}

void AMatchGameMode::SpawnCharacters(const TArray<AArenaPlayerStart*>& SpawnPoints)
{
    USmashCharacterInputData* InputData = LoadInputDataFromConfig();
    UInputMappingContext* InputMappingContext = LoadInputMappingContextFromConfig();

    for (AArenaPlayerStart* SpawnPoint : SpawnPoints)
    {
        const EAutoReceiveInput::Type InputType = SpawnPoint->AutoReceiveInput.GetValue();
        TSubclassOf<ASmashCharacter> SmashCharacterClass = GetSmashCharacterClassFromInputType(InputType);
        if (!SmashCharacterClass)
        {
            continue;
        }

        ASmashCharacter* NewCharacter = GetWorld()->SpawnActorDeferred<ASmashCharacter>(SmashCharacterClass, SpawnPoint->GetTransform());
        if (!NewCharacter)
        {
            continue;
        }

        NewCharacter->AutoPossessPlayer = SpawnPoint->AutoReceiveInput;
        
        NewCharacter->SetInputData(InputData);
        NewCharacter->SetInputMappingContext(InputMappingContext);
        
        NewCharacter->SetOrientX(SpawnPoint->GetStartOrientX());
        
        NewCharacter->FinishSpawning(SpawnPoint->GetTransform());

        CharactersInsideArena.Add(NewCharacter);
    }
}

TSubclassOf<ASmashCharacter> AMatchGameMode::GetSmashCharacterClassFromInputType(EAutoReceiveInput::Type InputType) const
{
    const UArenaSettings* ArenaSettings = GetDefault<UArenaSettings>();
    if (!ArenaSettings)
    {
        return nullptr;
    }

    switch (InputType)
    {
    case EAutoReceiveInput::Player0:
        return ArenaSettings->SmashCharacterClassP0;
    case EAutoReceiveInput::Player1:
        return ArenaSettings->SmashCharacterClassP1;
    case EAutoReceiveInput::Player2:
        return ArenaSettings->SmashCharacterClassP2;
    case EAutoReceiveInput::Player3:
        return ArenaSettings->SmashCharacterClassP3;
    default:
        return nullptr;
    }
}

