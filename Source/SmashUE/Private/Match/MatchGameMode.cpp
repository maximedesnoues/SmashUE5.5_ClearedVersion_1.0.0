// Fill out your copyright notice in the Description page of Project Settings.


#include "Match/MatchGameMode.h"
#include "Arena/ArenaPlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AMatchGameMode::BeginPlay()
{
    Super::BeginPlay();

    TArray<AArenaPlayerStart*> PlayerStartsPoints;
    FindPlayerStartActorsInArena(PlayerStartsPoints);

    for (AArenaPlayerStart* PlayerStartPoint : PlayerStartsPoints)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            3.f,
            FColor::Cyan,
            PlayerStartPoint->GetFName().ToString()
        );
    }
}

void AMatchGameMode::FindPlayerStartActorsInArena(TArray<AArenaPlayerStart*>& ResultsActors)
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AArenaPlayerStart::StaticClass(), FoundActors);

    for (int i = 0; i < FoundActors.Num(); ++i)
    {
        AArenaPlayerStart* ArenaPlayerStartActor = Cast<AArenaPlayerStart>(FoundActors[i]);
        if (ArenaPlayerStartActor == nullptr) continue;

        ResultsActors.Add(ArenaPlayerStartActor);
    }
}

