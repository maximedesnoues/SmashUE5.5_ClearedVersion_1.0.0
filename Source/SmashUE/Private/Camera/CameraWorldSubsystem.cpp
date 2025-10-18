// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraWorldSubsystem.h"

#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

void UCameraWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCameraWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	CameraMain = FindCameraByTag(TEXT("CameraMain"));
}

void UCameraWorldSubsystem::AddFollowTarget(AActor* FollowTarget)
{
	if (!FollowTarget)
	{
		return;
	}
	
	FollowTargets.AddUnique(FollowTarget);
}

void UCameraWorldSubsystem::RemoveFollowTarget(AActor* FollowTarget)
{
	if (!FollowTarget)
	{
		return;
	}
	
	FollowTargets.Remove(FollowTarget);
}

void UCameraWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickUpdateCameraPosition(DeltaTime);
}

TStatId UCameraWorldSubsystem::GetStatId() const
{
	return TStatId();
}

UCameraComponent* UCameraWorldSubsystem::FindCameraByTag(const FName& Tag) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor)
		{
			continue;
		}

		if (Actor->ActorHasTag(Tag))
		{
			if (UCameraComponent* CameraComponent = Actor->FindComponentByClass<UCameraComponent>())
			{
				return CameraComponent;
			}
		}
	}
	
	return nullptr;
}

FVector UCameraWorldSubsystem::CalculateAveragePositionBetweenTargets() const
{
	if (FollowTargets.Num() == 0)
	{
		return FVector::ZeroVector;
	}

	FVector SumOfPositions = FVector::ZeroVector;
	int TargetCount = 0;

	for (AActor* FollowTarget : FollowTargets)
	{
		if (IsValid(FollowTarget))
		{
			SumOfPositions += FollowTarget->GetActorLocation();
			++TargetCount;
		}
	}

	if (TargetCount == 0)
	{
		return FVector::ZeroVector;
	}

	return SumOfPositions / static_cast<float>(TargetCount);
}

void UCameraWorldSubsystem::TickUpdateCameraPosition(float DeltaTime)
{
	if (!CameraMain || FollowTargets.Num() == 0)
	{
		return;
	}

	static bool bOffsetInitialized = false;
	static FVector InitialOffset = FVector::ZeroVector;

	const FVector CurrentCameraPosition = CameraMain->GetComponentLocation();
	const FVector AverageTargetPosition = CalculateAveragePositionBetweenTargets();

	if (!bOffsetInitialized)
	{
		InitialOffset = CurrentCameraPosition - AverageTargetPosition;
		bOffsetInitialized = true;
	}

	const FVector DesiredCameraPosition = AverageTargetPosition + InitialOffset;
	const FVector NewCameraPosition = FMath::VInterpTo(CurrentCameraPosition, DesiredCameraPosition, DeltaTime, 3.f);

	CameraMain->SetWorldLocation(NewCameraPosition);
}

