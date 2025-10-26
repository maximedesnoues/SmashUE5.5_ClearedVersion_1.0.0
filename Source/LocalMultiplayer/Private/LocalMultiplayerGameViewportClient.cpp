// Fill out your copyright notice in the Description page of Project Settings.

#include "LocalMultiplayerGameViewportClient.h"

#include "LocalMultiplayerSettings.h"
#include "LocalMultiplayerSubsystem.h"

#include "GameFramework/PlayerInput.h"

#include "Kismet/GameplayStatics.h"

void ULocalMultiplayerGameViewportClient::PostInitProperties()
{
	Super::PostInitProperties();
	MaxSplitscreenPlayers = 8;
}

bool ULocalMultiplayerGameViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
    if (EventArgs.Event != IE_Pressed && EventArgs.Event != IE_Repeat && EventArgs.Event != IE_Released)
    {
        return Super::InputKey(EventArgs);
    }
    
    UGameInstance* GI = GetGameInstance();
    if (!GI)
    {
        return Super::InputKey(EventArgs);
    }

    UWorld* WorldPtr = GetWorld();
    if (!WorldPtr)
    {
        return Super::InputKey(EventArgs);
    }
    
    const ULocalMultiplayerSettings* LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();
    if (!LocalMultiplayerSettings)
    {
        return Super::InputKey(EventArgs);
    }
    
    ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GI->GetSubsystem<ULocalMultiplayerSubsystem>();
    if (!LocalMultiplayerSubsystem)
    {
        return Super::InputKey(EventArgs);
    }

    const ELocalMultiplayerInputMappingType MappingType = ELocalMultiplayerInputMappingType::InGame;

    if (!EventArgs.Key.IsGamepadKey())
    {
        const int KeyboardProfileIndex = LocalMultiplayerSettings->FindKeyboardProfileIndexFromKey(EventArgs.Key, MappingType);
        if (KeyboardProfileIndex == -1)
        {
            return Super::InputKey(EventArgs);
        }

        int PlayerIndex = LocalMultiplayerSubsystem->GetAssignedPlayerIndexFromKeyboardProfileIndex(KeyboardProfileIndex);
        if (PlayerIndex == -1)
        {
            PlayerIndex = LocalMultiplayerSubsystem->AssignNewPlayerToKeyboardProfileIndex(KeyboardProfileIndex);
            LocalMultiplayerSubsystem->AssignKeyboardInputMapping(PlayerIndex, KeyboardProfileIndex, MappingType);
        }

        if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldPtr, PlayerIndex))
        {
            return PC->InputKey(FInputKeyParams(EventArgs.Key, EventArgs.Event, EventArgs.AmountDepressed, false, EventArgs.InputDevice));
        }

        return Super::InputKey(EventArgs);
    }
    else
    {
        const int DeviceID = EventArgs.InputDevice.GetId();

        int PlayerIndex = LocalMultiplayerSubsystem->GetAssignedPlayerIndexFromGamepadDeviceID(DeviceID);
        if (PlayerIndex == -1)
        {
            PlayerIndex = LocalMultiplayerSubsystem->AssignNewPlayerToGamepadDeviceID(DeviceID);
            LocalMultiplayerSubsystem->AssignGamepadInputMapping(PlayerIndex, MappingType);
        }

        if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldPtr, PlayerIndex))
        {
            return PC->InputKey(FInputKeyParams(EventArgs.Key, EventArgs.Event, EventArgs.AmountDepressed, true, EventArgs.InputDevice));
        }

        return Super::InputKey(EventArgs);
    }
}

bool ULocalMultiplayerGameViewportClient::InputAxis(FViewport* InViewport, FInputDeviceId InputDevice, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
    UGameInstance* GI = GetGameInstance();
    if (!GI)
    {
        return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
    }

    UWorld* WorldPtr = GetWorld();
    if (!WorldPtr)
    {
        return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
    }

    const ULocalMultiplayerSettings* LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();
    if (!LocalMultiplayerSettings)
    {
        return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
    }

    ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GI->GetSubsystem<ULocalMultiplayerSubsystem>();
    if (!LocalMultiplayerSubsystem)
    {
        return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
    }

    const ELocalMultiplayerInputMappingType MappingType = ELocalMultiplayerInputMappingType::InGame;

    if (!bGamepad && !Key.IsGamepadKey())
    {
        const int KeyboardProfileIndex = LocalMultiplayerSettings->FindKeyboardProfileIndexFromKey(Key, MappingType);
        if (KeyboardProfileIndex == -1)
        {
            return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
        }

        int PlayerIndex = LocalMultiplayerSubsystem->GetAssignedPlayerIndexFromKeyboardProfileIndex(KeyboardProfileIndex);
        if (PlayerIndex == -1)
        {
            PlayerIndex = LocalMultiplayerSubsystem->AssignNewPlayerToKeyboardProfileIndex(KeyboardProfileIndex);
            LocalMultiplayerSubsystem->AssignKeyboardInputMapping(PlayerIndex, KeyboardProfileIndex, MappingType);
        }

        if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldPtr, PlayerIndex))
        {
            return PC->InputKey(FInputKeyParams(Key, Delta, DeltaTime, NumSamples, bGamepad, InputDevice));
        }

        return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
    }
    else
    {
        const int DeviceID = InputDevice.GetId();

        int PlayerIndex = LocalMultiplayerSubsystem->GetAssignedPlayerIndexFromGamepadDeviceID(DeviceID);
        if (PlayerIndex == -1)
        {
            PlayerIndex = LocalMultiplayerSubsystem->AssignNewPlayerToGamepadDeviceID(DeviceID);
            LocalMultiplayerSubsystem->AssignGamepadInputMapping(PlayerIndex, MappingType);
        }

        if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldPtr, PlayerIndex))
        {
            return PC->InputKey(FInputKeyParams(Key, Delta, DeltaTime, NumSamples, bGamepad, InputDevice));
        }

        return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
    }
}
