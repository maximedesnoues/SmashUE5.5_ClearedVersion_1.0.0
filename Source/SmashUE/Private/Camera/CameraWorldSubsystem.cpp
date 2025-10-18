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
	FVector NewCameraPosition = FMath::VInterpTo(CurrentCameraPosition, DesiredCameraPosition, DeltaTime, 3.f);

	ClampPositionIntoCameraBounds(NewCameraPosition);

	CameraMain->SetWorldLocation(NewCameraPosition);
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

AActor* UCameraWorldSubsystem::FindCameraBoundsActor()
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

void UCameraWorldSubsystem::GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax)
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

FVector UCameraWorldSubsystem::CalculateWorldPositionFromViewportPosition(const FVector2D& ViewportPosition)
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

