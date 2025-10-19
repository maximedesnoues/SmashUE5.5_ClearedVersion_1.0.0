// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraWorldSubsystem.h"

#include "Camera/CameraFollowTarget.h"

#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

void UCameraWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCameraWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	CameraMain = FindCameraByTag(TEXT("CameraMain"));

	if (AActor* CameraBoundsActor = FindCameraBoundsActor())
	{
		InitCameraBounds(CameraBoundsActor);
	}

	InitCameraZoomParameters();
}

void UCameraWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickUpdateCameraPosition(DeltaTime);
	TickUpdateCameraZoom(DeltaTime);
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

void UCameraWorldSubsystem::TickUpdateCameraPosition(float DeltaTime)
{
	if (!CameraMain || FollowTargets.Num() == 0)
	{
		return;
	}

	AActor* CameraOwner = CameraMain->GetOwner();
	if (!CameraOwner)
	{
		return;
	}

	static bool bOffsetInitialized = false;
	static FVector InitialOffset = FVector::ZeroVector;

	const FVector CurrentCameraPosition = CameraOwner->GetActorLocation();
	const FVector AverageTargetPosition = CalculateAveragePositionBetweenTargets();

	if (!bOffsetInitialized)
	{
		const FVector InitialComponentOffset = CameraMain->GetComponentLocation() - AverageTargetPosition;
		InitialOffset = FVector(InitialComponentOffset.X, 0.f, InitialComponentOffset.Z);
		bOffsetInitialized = true;
	}

	FVector DesiredCameraPosition = CurrentCameraPosition;
	DesiredCameraPosition.X = AverageTargetPosition.X + InitialOffset.X;
	DesiredCameraPosition.Z = AverageTargetPosition.Z + InitialOffset.Z;

	FVector NewCameraPosition = FMath::VInterpTo(CurrentCameraPosition, DesiredCameraPosition, DeltaTime, 3.f);

	ClampPositionIntoCameraBounds(NewCameraPosition);

	CameraOwner->SetActorLocation(NewCameraPosition);
}

void UCameraWorldSubsystem::TickUpdateCameraZoom(float DeltaTime)
{
	if (!CameraMain)
	{
		return;
	}

	const float GreatestDistanceBetweenTargets = CalculateGreatestDistanceBetweenTargets();

	const float InverseLerpDenominator = (CameraZoomDistanceBetweenTargetsMax - CameraZoomDistanceBetweenTargetsMin);

	float CurrentDistancePercent = 0.f;
	
	if (!FMath::IsNearlyZero(InverseLerpDenominator))
	{
		CurrentDistancePercent = (GreatestDistanceBetweenTargets - CameraZoomDistanceBetweenTargetsMin) / InverseLerpDenominator;
	}
	
	CurrentDistancePercent = FMath::Clamp(CurrentDistancePercent, 0.f, 1.f);

	if (AActor* CameraOwner = CameraMain->GetOwner())
	{
		const float TargetOwnerY = FMath::Lerp(CameraZoomYMin, CameraZoomYMax, CurrentDistancePercent);

		FVector CameraOwnerLocation = CameraOwner->GetActorLocation();
		CameraOwnerLocation.Y = FMath::FInterpTo(CameraOwnerLocation.Y, TargetOwnerY, DeltaTime, 6.f);

		CameraOwner->SetActorLocation(CameraOwnerLocation);
	}
}

void UCameraWorldSubsystem::AddFollowTarget(UObject* FollowTarget)
{
	if (!FollowTarget)
	{
		return;
	}

	FollowTargets.AddUnique(FollowTarget);
}

void UCameraWorldSubsystem::RemoveFollowTarget(UObject* FollowTarget)
{
	if (!FollowTarget)
	{
		return;
	}

	FollowTargets.Remove(FollowTarget);
}

FVector UCameraWorldSubsystem::CalculateAveragePositionBetweenTargets() const
{
	if (FollowTargets.Num() == 0)
	{
		return FVector::ZeroVector;
	}

	FVector SumOfPositions = FVector::ZeroVector;
	int TargetCount = 0;

	for (UObject* Object : FollowTargets)
	{
		if (!IsValid(Object))
		{
			continue;
		}

		if (ICameraFollowTarget* CameraFollowTarget = Cast<ICameraFollowTarget>(Object))
		{
			if (CameraFollowTarget->IsFollowable())
			{
				SumOfPositions += CameraFollowTarget->GetFollowPosition();
				++TargetCount;
			}
		}
	}

	if (TargetCount == 0)
	{
		return FVector::ZeroVector;
	}

	return SumOfPositions / static_cast<float>(TargetCount);
}

float UCameraWorldSubsystem::CalculateGreatestDistanceBetweenTargets() const
{
	float GreatestDistance = 0.f;

	if (FollowTargets.Num() < 2)
	{
		return GreatestDistance;
	}

	TArray<FVector> TargetPositions;
	TargetPositions.Reserve(FollowTargets.Num());

	for (UObject* Object : FollowTargets)
	{
		if (!IsValid(Object))
		{
			continue;
		}

		if (const ICameraFollowTarget* CameraFollowTarget = Cast<const ICameraFollowTarget>(Object))
		{
			if (CameraFollowTarget->IsFollowable())
			{
				TargetPositions.Add(CameraFollowTarget->GetFollowPosition());
			}
		}
	}

	if (TargetPositions.Num() < 2)
	{
		return GreatestDistance;
	}

	for (int i = 0; i < TargetPositions.Num(); ++i)
	{
		for (int j = i + 1; j < TargetPositions.Num(); ++j)
		{
			const float CurrentDistance = FVector::Dist(TargetPositions[i], TargetPositions[j]);
			if (CurrentDistance > GreatestDistance)
			{
				GreatestDistance = CurrentDistance;
			}
		}
	}

	return GreatestDistance;
}

AActor* UCameraWorldSubsystem::FindCameraBoundsActor() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}
	
	const FName Tag(TEXT("CameraBounds"));

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor)
		{
			continue;
		}

		if (Actor->ActorHasTag(Tag))
		{
			return Actor;
		}
	}
	
	return nullptr;
}

void UCameraWorldSubsystem::InitCameraBounds(AActor* CameraBoundsActor)
{
	if (!CameraBoundsActor)
	{
		return;
	}

	FVector BoundsCenter;
	FVector BoundsExtents;
	CameraBoundsActor->GetActorBounds(false, BoundsCenter, BoundsExtents);

	CameraBoundsMin = FVector2D(BoundsCenter.X - BoundsExtents.X, BoundsCenter.Z - BoundsExtents.Z);
	CameraBoundsMax = FVector2D(BoundsCenter.X + BoundsExtents.X, BoundsCenter.Z + BoundsExtents.Z);

	CameraBoundsYProjectionCenter = BoundsCenter.Y;
}

void UCameraWorldSubsystem::GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax) const
{
	// Find Viewport
	UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
	if (!ViewportClient)
	{
		return;
	}
	FViewport* Viewport = ViewportClient->Viewport;
	if (!Viewport)
	{
		return;
	}

	// Calculate Viewport Rect according to Camera Aspect Ratio and Viewport ViewRect
	FIntRect ViewRect(Viewport->GetInitialPositionXY(), Viewport->GetInitialPositionXY() + Viewport->GetSizeXY());
	FIntRect ViewportRect = Viewport->CalculateViewExtents(CameraMain->AspectRatio, ViewRect);

	// Fill output parameters with ViewportRect
	OutViewportBoundsMin.X = ViewportRect.Min.X;
	OutViewportBoundsMin.Y = ViewportRect.Min.Y;
	OutViewportBoundsMax.X = ViewportRect.Max.X;
	OutViewportBoundsMax.Y = ViewportRect.Max.Y;
}

FVector UCameraWorldSubsystem::CalculateWorldPositionFromViewportPosition(const FVector2D& ViewportPosition) const
{
	if (!CameraMain)
	{
		return FVector::ZeroVector;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController)
	{
		return FVector::ZeroVector;
	}

	const float YDistanceToCenter = CameraMain->GetOwner()->GetActorLocation().Y - CameraBoundsYProjectionCenter;

	FVector WorldPosition;
	FVector CameraWorldProjectDir;

	UGameplayStatics::DeprojectScreenToWorld(PlayerController, ViewportPosition, WorldPosition, CameraWorldProjectDir);

	WorldPosition += CameraWorldProjectDir * YDistanceToCenter;

	return WorldPosition;
}

void UCameraWorldSubsystem::ClampPositionIntoCameraBounds(FVector& Position)
{
	FVector2D ViewportBoundsMin, ViewportBoundsMax;
	GetViewportBounds(ViewportBoundsMin, ViewportBoundsMax);

	const FVector WorldBoundsMin = CalculateWorldPositionFromViewportPosition(ViewportBoundsMin);
	const FVector WorldBoundsMax = CalculateWorldPositionFromViewportPosition(ViewportBoundsMax);

	const float ViewportWidth = FMath::Abs(WorldBoundsMax.X - WorldBoundsMin.X);
	const float ViewportHeight = FMath::Abs(WorldBoundsMax.Z - WorldBoundsMin.Z);

	if (ViewportWidth < KINDA_SMALL_NUMBER || ViewportHeight < KINDA_SMALL_NUMBER)
	{
		return;
	}

	const float HalfViewportWidth = ViewportWidth * 0.5f;
	const float HalfViewportHeight = ViewportHeight * 0.5f;

	const float AllowedCameraMinX = CameraBoundsMin.X + HalfViewportWidth;
	const float AllowedCameraMaxX = CameraBoundsMax.X - HalfViewportWidth;

	const float AllowedCameraMinZ = CameraBoundsMin.Y + HalfViewportHeight;
	const float AllowedCameraMaxZ = CameraBoundsMax.Y - HalfViewportHeight;

	if (AllowedCameraMinX < AllowedCameraMaxX && AllowedCameraMinZ < AllowedCameraMaxZ)
	{
		Position.X = FMath::Clamp(Position.X, AllowedCameraMinX, AllowedCameraMaxX);
		Position.Z = FMath::Clamp(Position.Z, AllowedCameraMinZ, AllowedCameraMaxZ);
	}
}

void UCameraWorldSubsystem::InitCameraZoomParameters()
{
	AActor* CameraDistanceMinActor = FindCameraDistanceMinActor();
	AActor* CameraDistanceMaxActor = FindCameraDistanceMaxActor();

	if (CameraDistanceMinActor)
	{
		CameraZoomYMin = CameraDistanceMinActor->GetActorLocation().Y;
	}

	if (CameraDistanceMaxActor)
	{
		CameraZoomYMax = CameraDistanceMaxActor->GetActorLocation().Y;
	}
}

AActor* UCameraWorldSubsystem::FindCameraDistanceMinActor() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	const FName Tag(TEXT("CameraDistanceMin"));

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor && Actor->ActorHasTag(Tag))
		{
			return Actor;
		}
	}
	
	return nullptr;
}

AActor* UCameraWorldSubsystem::FindCameraDistanceMaxActor() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	const FName Tag(TEXT("CameraDistanceMax"));

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor && Actor->ActorHasTag(Tag))
		{
			return Actor;
		}
	}
	
	return nullptr;
}

