// Fill out your copyright notice in the Description page of Project Settings.


#include "Match/MatchGameMode.h"

#include "Arena/ArenaPlayerStart.h"
#include "Arena/ArenaSettings.h"
#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterInputData.h"
#include "Characters/SmashCharacterSettings.h"

#include "LocalMultiplayerSettings.h"
#include "LocalMultiplayerSubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void AMatchGameMode::BeginPlay()
{
    Super::BeginPlay();

    CreateAndInitPlayers();

    TArray<AArenaPlayerStart*> PlayerStartsPoints;
    FindPlayerStartActorsInArena(PlayerStartsPoints);
    SpawnCharacters(PlayerStartsPoints);

    PossessSpawnedCharacters();
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

void AMatchGameMode::CreateAndInitPlayers() const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    
    UGameInstance* GameInstance = World->GetGameInstance();
    if (!GameInstance)
    {
        return;
    }

    ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GameInstance->GetSubsystem<ULocalMultiplayerSubsystem>();
    if (!LocalMultiplayerSubsystem)
    {
        return;
    }

    LocalMultiplayerSubsystem->CreateAndInitPlayers(ELocalMultiplayerInputMappingType::InGame);
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

        // NewCharacter->AutoPossessPlayer = SpawnPoint->AutoReceiveInput;
        
        NewCharacter->SetInputData(InputData);
        // NewCharacter->SetInputMappingContext(InputMappingContext);
        
        NewCharacter->SetOrientX(SpawnPoint->GetStartOrientX());
        
        NewCharacter->FinishSpawning(SpawnPoint->GetTransform());

        CharactersInsideArena.Add(NewCharacter);
    }
}

void AMatchGameMode::PossessSpawnedCharacters()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        const int LocalPlayerCount = GameInstance->GetLocalPlayers().Num();
        const int CharacterCount = CharactersInsideArena.Num();
        const int CountToAssign = FMath::Min(LocalPlayerCount, CharacterCount);

        for (int i = 0; i < CountToAssign; ++i)
        {
            if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, i))
            {
                if (ASmashCharacter* Character = CharactersInsideArena[i])
                {
                    PlayerController->Possess(Character);
                }
            }
        }
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

