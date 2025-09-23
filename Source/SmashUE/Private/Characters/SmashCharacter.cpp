// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SmashCharacter.h"

#include "Characters/SmashCharacterInputData.h"
#include "Characters/SmashCharacterStateMachine.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"

// Sets default values
ASmashCharacter::ASmashCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASmashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CreateStateMachine();
	InitStateMachine();
}

// Called every frame
void ASmashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TickStateMachine(DeltaTime);
	RotateMeshUsingOrientX();
}

// Called to bind functionality to input
void ASmashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	SetupMappingContextIntoController();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		BindInputMoveXActions(EnhancedInputComponent);
		BindInputJumpAction(EnhancedInputComponent);
		BindInputFallFastAction(EnhancedInputComponent);
	}
}

float ASmashCharacter::GetOrientX() const
{
	return OrientX;
}

void ASmashCharacter::SetOrientX(float NewOrientX)
{
	OrientX = NewOrientX;
}

void ASmashCharacter::RotateMeshUsingOrientX() const
{
	if (!GetMesh())
	{
		return;
	}

	FRotator Rotation = GetMesh()->GetRelativeRotation();
	Rotation.Yaw = -90.f * OrientX;
	GetMesh()->SetRelativeRotation(Rotation);
}

void ASmashCharacter::CreateStateMachine()
{
	if (!StateMachine)
	{
		StateMachine = NewObject<USmashCharacterStateMachine>(this);
	}
}

void ASmashCharacter::InitStateMachine()
{
	if (!StateMachine)
	{
		return;
	}
	
	StateMachine->Init(this);
}

void ASmashCharacter::TickStateMachine(float DeltaTime) const
{
	if (!StateMachine)
	{
		return;
	}
	
	StateMachine->Tick(DeltaTime);
}

void ASmashCharacter::SetInputData(USmashCharacterInputData* InInputData)
{
	InputData = InInputData;
}

void ASmashCharacter::SetInputMappingContext(UInputMappingContext* InInputMappingContext)
{
	InputMappingContext = InInputMappingContext;
}

void ASmashCharacter::SetupMappingContextIntoController() const
{
	const APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController)
	{
		return;
	}

	const ULocalPlayer* Player = PlayerController->GetLocalPlayer();
	if (!Player)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* InputSystem = Player->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		if (InputMappingContext)
		{
			InputSystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

float ASmashCharacter::GetInputMoveX() const
{
	return InputMoveX;
}

float ASmashCharacter::GetInputMoveXFast() const
{
	return InputMoveXFast;
}

void ASmashCharacter::BindInputMoveXActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (!InputData)
	{
		return;
	}

	if (InputData->InputActionMoveX)
	{
		EnhancedInputComponent->BindAction(InputData->InputActionMoveX, ETriggerEvent::Triggered, this, &ASmashCharacter::OnInputMoveX);
		EnhancedInputComponent->BindAction(InputData->InputActionMoveX, ETriggerEvent::Completed, this, &ASmashCharacter::OnInputMoveX);
	}

	if (InputData->InputActionMoveXFast)
	{
		EnhancedInputComponent->BindAction(InputData->InputActionMoveXFast, ETriggerEvent::Triggered, this, &ASmashCharacter::OnInputMoveXFast);
	}
}

void ASmashCharacter::OnInputMoveX(const FInputActionValue& InputActionValue)
{
	InputMoveX = InputActionValue.Get<float>();
}

void ASmashCharacter::OnInputMoveXFast(const FInputActionValue& InputActionValue)
{
	InputMoveXFast = InputActionValue.Get<float>();
	InputMoveXFastEvent.Broadcast(InputMoveXFast);
}

void ASmashCharacter::BindInputJumpAction(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (!InputData)
	{
		return;
	}

	if (InputData->InputActionJump)
	{
		EnhancedInputComponent->BindAction(InputData->InputActionJump, ETriggerEvent::Started, this, &ASmashCharacter::OnInputJump);
	}
}

void ASmashCharacter::OnInputJump(const FInputActionValue& InputActionValue)
{
	InputJumpEvent.Broadcast();
}

void ASmashCharacter::BindInputFallFastAction(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (!InputData)
	{
		return;
	}

	if (InputData->InputActionFallFast)
	{
		EnhancedInputComponent->BindAction(InputData->InputActionFallFast, ETriggerEvent::Triggered, this, &ASmashCharacter::OnInputFallFast);
	}
}

void ASmashCharacter::OnInputFallFast(const FInputActionValue& InputActionValue)
{
	InputFallFast = InputActionValue.Get<float>();
	InputFallFastEvent.Broadcast(InputFallFast);
}

