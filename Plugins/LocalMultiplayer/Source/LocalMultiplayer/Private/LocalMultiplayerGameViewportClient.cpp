// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalMultiplayerGameViewportClient.h"

#include "LocalMultiplayerSettings.h"
#include "LocalMultiplayerSubsystem.h"

#include "Engine/EngineBaseTypes.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

#include "InputCoreTypes.h"
#include "InputKeyEventArgs.h"

void ULocalMultiplayerGameViewportClient::PostInitProperties()
{
	Super::PostInitProperties();
	
	MaxSplitscreenPlayers = 8;
}

bool ULocalMultiplayerGameViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	if (EventArgs.Event != IE_Pressed && EventArgs.Event != IE_Repeat)
	{
		return Super::InputKey(EventArgs);
	}

	UWorld* WorldPtr = GetWorld();
	if (!WorldPtr)
	{
		return Super::InputKey(EventArgs);
	}

	UGameInstance* GameInstancePtr = WorldPtr->GetGameInstance();
	if (!GameInstancePtr)
	{
		return Super::InputKey(EventArgs);
	}

	ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GameInstancePtr->GetSubsystem<ULocalMultiplayerSubsystem>();
	if (!LocalMultiplayerSubsystem)
	{
		return Super::InputKey(EventArgs);
	}

	ULocalMultiplayerSettings* LocalMultiplayerSettings = GetMutableDefault<ULocalMultiplayerSettings>();
	if (!LocalMultiplayerSettings)
	{
		return Super::InputKey(EventArgs);
	}

	const ELocalMultiplayerInputMappingType MappingType = ELocalMultiplayerInputMappingType::InGame;

	const int KeyboardProfileIndex = LocalMultiplayerSettings->FindKeyboardProfileIndexFromKey(EventArgs.Key, MappingType);
	if (KeyboardProfileIndex != -1)
	{
		const int PlayerIndex = LocalMultiplayerSubsystem->AssignNewPlayerToKeyboardProfileIndex(KeyboardProfileIndex);
		if (PlayerIndex != -1)
		{
			LocalMultiplayerSubsystem->AssignKeyboardInputMapping(PlayerIndex, KeyboardProfileIndex, MappingType);
		}
	}

	if (EventArgs.Key.IsGamepadKey())
	{
		const int DeviceID = EventArgs.InputDevice.GetId();
		const int PlayerIndex = LocalMultiplayerSubsystem->AssignNewPlayerToGamepadDeviceID(DeviceID);
		if (PlayerIndex != -1)
		{
			LocalMultiplayerSubsystem->AssignGamepadInputMapping(PlayerIndex, MappingType);
		}
	}

	return Super::InputKey(EventArgs);
}

bool ULocalMultiplayerGameViewportClient::InputAxis(FViewport* InViewport, FInputDeviceId InputDevice, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	constexpr float AssignThreshold = KINDA_SMALL_NUMBER;
	if (FMath::Abs(Delta) <= AssignThreshold)
	{
		return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
	}

	UWorld* WorldPtr = GetWorld();
	if (!WorldPtr)
	{
		return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
	}

	UGameInstance* GameInstancePtr = WorldPtr->GetGameInstance();
	if (!GameInstancePtr)
	{
		return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
	}

	ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GameInstancePtr->GetSubsystem<ULocalMultiplayerSubsystem>();
	if (!LocalMultiplayerSubsystem)
	{
		return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
	}

	ULocalMultiplayerSettings* LocalMultiplayerSettings = GetMutableDefault<ULocalMultiplayerSettings>();
	if (!LocalMultiplayerSettings)
	{
		return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
	}

	const ELocalMultiplayerInputMappingType MappingType = ELocalMultiplayerInputMappingType::InGame;

	const int KeyboardProfileIndex = LocalMultiplayerSettings->FindKeyboardProfileIndexFromKey(Key, MappingType);
	if (KeyboardProfileIndex != -1)
	{
		const int PlayerIndex = LocalMultiplayerSubsystem->AssignNewPlayerToKeyboardProfileIndex(KeyboardProfileIndex);
		if (PlayerIndex != -1)
		{
			LocalMultiplayerSubsystem->AssignKeyboardInputMapping(PlayerIndex, KeyboardProfileIndex, MappingType);
		}
	}

	if (bGamepad || Key.IsGamepadKey())
	{
		const int DeviceID = InputDevice.GetId();
		const int PlayerIndex = LocalMultiplayerSubsystem->AssignNewPlayerToGamepadDeviceID(DeviceID);
		if (PlayerIndex != -1)
		{
			LocalMultiplayerSubsystem->AssignGamepadInputMapping(PlayerIndex, MappingType);
		}
	}

	return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
}

