// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera/CameraWorldSubsystem.h"

#include "Camera/CameraFollowTarget.h"
#include "Camera/CameraSettings.h"

#include "Camera/CameraComponent.h"

#include "Kismet/GameplayStatics.h"

#include "EngineUtils.h"

void UCameraWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCameraWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	const UCameraSettings* CameraSettings = GetDefault<UCameraSettings>();
	if (CameraSettings)
	{
		CameraMain = FindCameraByTag(CameraSettings->CameraMainTag);
	}
	else
	{
		CameraMain = FindCameraByTag(TEXT("CameraMain"));
	}

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

	if (Tag.IsNone())
	{
		return nullptr;
	}

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
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
	if (!CameraMain)
	{
		return;
	}

	if (FollowTargets.Num() == 0)
	{
		return;
	}

	const UCameraSettings* CameraSettings = GetDefault<UCameraSettings>();
	if (!CameraSettings)
	{
		return;
	}

	AActor* CameraOwner = CameraMain->GetOwner();
	if (!CameraOwner)
	{
		return;
	}

	const FVector AveragePositionBetweenTargets = CalculateAveragePositionBetweenTargets();

	const float Alpha = 1.f - FMath::Exp(-CameraSettings->PositionDampingFactor * DeltaTime);

	FVector CurrentCameraLocation = CameraOwner->GetActorLocation();
	CurrentCameraLocation.X = FMath::Lerp(CurrentCameraLocation.X, AveragePositionBetweenTargets.X, Alpha);
	CurrentCameraLocation.Z = FMath::Lerp(CurrentCameraLocation.Z, AveragePositionBetweenTargets.Z, Alpha);

	ClampPositionIntoCameraBounds(CurrentCameraLocation);

	CameraOwner->SetActorLocation(CurrentCameraLocation);
}

void UCameraWorldSubsystem::TickUpdateCameraZoom(float DeltaTime)
{
	if (!CameraMain)
	{
		return;
	}

	const UCameraSettings* CameraSettings = GetDefault<UCameraSettings>();
	if (!CameraSettings)
	{
		return;
	}

	AActor* CameraOwner = CameraMain->GetOwner();
	if (!CameraOwner)
	{
		return;
	}

	const float GreatestDistanceBetweenTargets = CalculateGreatestDistanceBetweenTargets();

	const float DistanceRange = CameraSettings->DistanceBetweenTargetsMax - CameraSettings->DistanceBetweenTargetsMin;
	float CurrentDistancePercent = 0.f;
	
	if (!FMath::IsNearlyZero(DistanceRange))
	{
		CurrentDistancePercent = (GreatestDistanceBetweenTargets - CameraSettings->DistanceBetweenTargetsMin) / DistanceRange;
	}
	
	CurrentDistancePercent = FMath::Clamp(CurrentDistancePercent, 0.f, 1.f);

	const float DesiredCameraY = FMath::Lerp(CameraZoomYMin, CameraZoomYMax, CurrentDistancePercent);

	const float Alpha = 1.f - FMath::Exp(-CameraSettings->SizeDampingFactor * DeltaTime);

	FVector CurrentCameraLocation = CameraOwner->GetActorLocation();
	CurrentCameraLocation.Y = FMath::Lerp(CurrentCameraLocation.Y, DesiredCameraY, Alpha);

	CameraOwner->SetActorLocation(CurrentCameraLocation);
}

void UCameraWorldSubsystem::AddFollowTarget(UObject* FollowTarget)
{
	if (FollowTarget)
	{
		FollowTargets.AddUnique(FollowTarget);
	}
}

void UCameraWorldSubsystem::RemoveFollowTarget(UObject* FollowTarget)
{
	if (FollowTarget)
	{
		FollowTargets.Remove(FollowTarget);
	}
}

FVector UCameraWorldSubsystem::CalculateAveragePositionBetweenTargets() const
{
	if (FollowTargets.Num() == 0)
	{
		return FVector::ZeroVector;
	}

	FVector SumOfTargetPositions = FVector::ZeroVector;
	int ValidTargetCount = 0;

	for (UObject* Object : FollowTargets)
	{
		if (const ICameraFollowTarget* CameraFollowTarget = Cast<ICameraFollowTarget>(Object))
		{
			if (CameraFollowTarget->IsFollowable())
			{
				SumOfTargetPositions += CameraFollowTarget->GetFollowPosition();
				++ValidTargetCount;
			}
		}
	}

	if (ValidTargetCount == 0)
	{
		return FVector::ZeroVector;
	}

	return SumOfTargetPositions / ValidTargetCount;
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
		if (const ICameraFollowTarget* CameraFollowTarget = Cast<ICameraFollowTarget>(Object))
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

	for (int i = 0; i < TargetPositions.Num() - 1; ++i)
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

	FName CameraBounds;

	const UCameraSettings* CameraSettings = GetDefault<UCameraSettings>();
	if (CameraSettings)
	{
		CameraBounds = CameraSettings->CameraBoundsTag;
	}
	else
	{
		CameraBounds = FName(TEXT("CameraBounds"));
	}

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor->ActorHasTag(CameraBounds))
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

	FVector WorldPosition;
	FVector CameraWorldProjectDir;

	UGameplayStatics::DeprojectScreenToWorld(PlayerController, ViewportPosition, WorldPosition, CameraWorldProjectDir);

	const float YDistanceToCenter = CameraMain->GetOwner()->GetActorLocation().Y - CameraBoundsYProjectionCenter;

	WorldPosition += CameraWorldProjectDir * YDistanceToCenter;

	return WorldPosition;
}

void UCameraWorldSubsystem::ClampPositionIntoCameraBounds(FVector& Position) const
{
	FVector2D ViewportBoundsMin, ViewportBoundsMax;
	GetViewportBounds(ViewportBoundsMin, ViewportBoundsMax);

	const FVector WorldBoundsMin = CalculateWorldPositionFromViewportPosition(ViewportBoundsMin);
	const FVector WorldBoundsMax = CalculateWorldPositionFromViewportPosition(ViewportBoundsMax);

	const float ViewportWidth = FMath::Abs(WorldBoundsMax.X - WorldBoundsMin.X);
	const float ViewportHeight = FMath::Abs(WorldBoundsMax.Z - WorldBoundsMin.Z);

	const float HalfViewportWidth = ViewportWidth * 0.5f;
	const float HalfViewportHeight = ViewportHeight * 0.5f;

	const float MinAllowedX = CameraBoundsMin.X + HalfViewportWidth;
	const float MaxAllowedX = CameraBoundsMax.X - HalfViewportWidth;
	const float MinAllowedZ = CameraBoundsMin.Y + HalfViewportHeight;
	const float MaxAllowedZ = CameraBoundsMax.Y - HalfViewportHeight;

	if (MinAllowedX < MaxAllowedX && MinAllowedZ < MaxAllowedZ)
	{
		Position.X = FMath::Clamp(Position.X, MinAllowedX, MaxAllowedX);
		Position.Z = FMath::Clamp(Position.Z, MinAllowedZ, MaxAllowedZ);
	}
}

void UCameraWorldSubsystem::InitCameraZoomParameters()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FName CameraDistanceMin;
	FName CameraDistanceMax;

	const UCameraSettings* CameraSettings = GetDefault<UCameraSettings>();
	if (CameraSettings)
	{
		CameraDistanceMin = CameraSettings->CameraDistanceMinTag;
		CameraDistanceMax = CameraSettings->CameraDistanceMaxTag;
	}
	else
	{
		CameraDistanceMin = FName(TEXT("CameraDistanceMin"));
		CameraDistanceMax = FName(TEXT("CameraDistanceMax"));
	}

	bool bFoundCameraDistanceMinActor = false;
	bool bFoundCameraDistanceMaxActor = false;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!bFoundCameraDistanceMinActor && Actor->ActorHasTag(CameraDistanceMin))
		{
			CameraZoomYMin = Actor->GetActorLocation().Y;
			bFoundCameraDistanceMinActor = true;
		}
		else if (!bFoundCameraDistanceMaxActor && Actor->ActorHasTag(CameraDistanceMax))
		{
			CameraZoomYMax = Actor->GetActorLocation().Y;
			bFoundCameraDistanceMaxActor = true;
		}

		if (bFoundCameraDistanceMinActor && bFoundCameraDistanceMaxActor)
		{
			break;
		}
	}

	if (CameraZoomYMin > CameraZoomYMax)
	{
		Swap(CameraZoomYMin, CameraZoomYMax);
	}
}
