// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputMappingContext.h"
#include "SmashCharacter.generated.h"

class USmashCharacterInputData;
class USmashCharacterStateMachine;

class UEnhancedInputComponent;

struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputMoveXFastEvent, float, InputMoveXFast);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputJumpEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputFallFastEvent, float, InputFallFast);

UCLASS()
class SMASHUE_API ASmashCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Unreal Default

public:
	// Sets default values for this character's properties
	ASmashCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma endregion

#pragma region Orient

public:
	float GetOrientX() const;
	void SetOrientX(float NewOrientX);

protected:
	UPROPERTY(BlueprintReadOnly)
	float OrientX = 1.f;

	void RotateMeshUsingOrientX() const;

#pragma endregion

#pragma region State Machine

public:
	void CreateStateMachine();
	void InitStateMachine();
	void TickStateMachine(float DeltaTime) const;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USmashCharacterStateMachine> StateMachine = nullptr;

#pragma endregion

#pragma region Input Data / Mapping Context

public:
	void SetInputData(USmashCharacterInputData* InInputData);
	void SetInputMappingContext(UInputMappingContext* InInputMappingContext);

protected:
	UPROPERTY()
	TObjectPtr<USmashCharacterInputData> InputData = nullptr;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;

	void SetupMappingContextIntoController() const;

#pragma endregion

#pragma region Input Move X

public:
	UPROPERTY()
	FInputMoveXFastEvent InputMoveXFastEvent;

	float GetInputMoveX() const;
	float GetInputMoveXFast() const;

protected:
	UPROPERTY()
	float InputMoveX = 0.f;

	UPROPERTY()
	float InputMoveXFast = 0.f;

private:
	void BindInputMoveXActions(UEnhancedInputComponent* EnhancedInputComponent);
	void OnInputMoveX(const FInputActionValue& InputActionValue);
	void OnInputMoveXFast(const FInputActionValue& InputActionValue);

#pragma endregion

#pragma region Input Jump

public:
	UPROPERTY()
	FInputJumpEvent InputJumpEvent;

private:
	void BindInputJumpAction(UEnhancedInputComponent* EnhancedInputComponent);
	void OnInputJump(const FInputActionValue& InputActionValue);

#pragma endregion

#pragma region Input Fall Fast

public:
	UPROPERTY()
	FInputFallFastEvent InputFallFastEvent;

protected:
	UPROPERTY()
	float InputFallFast = 0.f;

private:
	void BindInputFallFastAction(UEnhancedInputComponent* EnhancedInputComponent);
	void OnInputFallFast(const FInputActionValue& InputActionValue);

#pragma endregion
};
