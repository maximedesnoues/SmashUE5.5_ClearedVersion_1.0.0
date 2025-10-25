// Fill out your copyright notice in the Description page of Project Settings.

#include "LocalMultiplayerSubsystem.h"

#include "LocalMultiplayerSettings.h"

#include "EnhancedInputSubsystems.h"

#include "Kismet/GameplayStatics.h"

void ULocalMultiplayerSubsystem::CreateAndInitPlayers(ELocalMultiplayerInputMappingType MappingType)
{
	const ULocalMultiplayerSettings* LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();
	if (!LocalMultiplayerSettings)
	{
		return;
	}

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const int NbKeyboardProfiles = LocalMultiplayerSettings->GetNbKeyboardProfiles();
    const int NbMaxGamepads = LocalMultiplayerSettings->NbMaxGamepads;
    const int ExistingPlayers = GetGameInstance()->GetLocalPlayers().Num();

    for (int i = 0; i < NbKeyboardProfiles; ++i)
    {
        const int ControllerId = ExistingPlayers + i;
        UGameplayStatics::CreatePlayer(World, ControllerId, true);
    }

    for (int i = 0; i < NbMaxGamepads; ++i)
    {
        const int ControllerId = ExistingPlayers + NbKeyboardProfiles + i;
        UGameplayStatics::CreatePlayer(World, ControllerId, true);
    }
}

int ULocalMultiplayerSubsystem::GetAssignedPlayerIndexFromKeyboardProfileIndex(int KeyboardProfileIndex) const
{
    if (const int* PlayerIndex = PlayerIndexFromKeyboardProfileIndex.Find(KeyboardProfileIndex))
    {
        return *PlayerIndex;
    }
    
    return -1;
}

int ULocalMultiplayerSubsystem::GetAssignedPlayerIndexFromGamepadDeviceID(int DeviceID) const
{
    if (const int* PlayerIndex = PlayerIndexFromGamepadProfileIndex.Find(DeviceID))
    {
        return *PlayerIndex;
    }
    
    return -1;
}

int ULocalMultiplayerSubsystem::AssignNewPlayerToKeyboardProfileIndex(int KeyboardProfileIndex)
{
    const int AssignedPlayerIndex = LastAssignedPlayerIndex++;
    PlayerIndexFromKeyboardProfileIndex.Add(KeyboardProfileIndex, AssignedPlayerIndex);
    return AssignedPlayerIndex;
}

int ULocalMultiplayerSubsystem::AssignNewPlayerToGamepadDeviceID(int DeviceID)
{
    const int AssignedPlayerIndex = LastAssignedPlayerIndex++;
    PlayerIndexFromGamepadProfileIndex.Add(DeviceID, AssignedPlayerIndex);
    return AssignedPlayerIndex;
}

void ULocalMultiplayerSubsystem::AssignKeyboardInputMapping(int PlayerIndex, int KeyboardProfileIndex, ELocalMultiplayerInputMappingType MappingType) const
{
    const ULocalMultiplayerSettings* LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();
    if (!LocalMultiplayerSettings || !LocalMultiplayerSettings->KeyboardProfilesData.IsValidIndex(KeyboardProfileIndex))
    {
        return;
    }

    UInputMappingContext* IMC = LocalMultiplayerSettings->KeyboardProfilesData[KeyboardProfileIndex].GetIMCFromType(MappingType);
    if (!IMC)
    {
        return;
    }

    UGameInstance* GI = GetGameInstance();
    if (!GI)
    {
        return;
    }

    ULocalPlayer* LP = GI->GetLocalPlayerByIndex(PlayerIndex);
    if (!LP)
    {
        return;
    }

    if (UEnhancedInputLocalPlayerSubsystem* EIS = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
    {
        if (!EIS->HasMappingContext(IMC))
        {
            FModifyContextOptions Options;
            Options.bForceImmediately = true;
            EIS->AddMappingContext(IMC, 0, Options);
        }
    }
}

void ULocalMultiplayerSubsystem::AssignGamepadInputMapping(int PlayerIndex, ELocalMultiplayerInputMappingType MappingType) const
{
    const ULocalMultiplayerSettings* LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();
    if (!LocalMultiplayerSettings)
    {
        return;
    }

    UInputMappingContext* IMC = LocalMultiplayerSettings->GamepadProfileData.GetIMCFromType(MappingType);
    if (!IMC)
    {
        return;
    }

    UGameInstance* GI = GetGameInstance();
    if (!GI)
    {
        return;
    }

    ULocalPlayer* LP = GI->GetLocalPlayerByIndex(PlayerIndex);
    if (!LP)
    {
        return;
    }

    if (UEnhancedInputLocalPlayerSubsystem* EIS = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
    {
        if (!EIS->HasMappingContext(IMC))
        {
            FModifyContextOptions Options;
            Options.bForceImmediately = true;
            EIS->AddMappingContext(IMC, 0, Options);
        }
    }
}
