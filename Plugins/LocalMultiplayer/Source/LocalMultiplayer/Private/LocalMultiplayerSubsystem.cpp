// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalMultiplayerSubsystem.h"

#include "LocalMultiplayerSettings.h"

#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

void ULocalMultiplayerSubsystem::CreateAndInitPlayers(ELocalMultiplayerInputMappingType MappingType)
{
	const ULocalMultiplayerSettings* LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();
	if (!LocalMultiplayerSettings)
	{
		return;
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	const int TargetPlayers = LocalMultiplayerSettings->GetNbKeyboardProfiles() + LocalMultiplayerSettings->NbMaxGamepads;

	while (GameInstance->GetLocalPlayers().Num() < TargetPlayers)
	{
		const int ControllerId = GameInstance->GetLocalPlayers().Num();
		FString OutError;
		GameInstance->CreateLocalPlayer(ControllerId, OutError, true);
	}

	LastAssignedPlayerIndex = 0;
	PlayerIndexFromKeyboardProfileIndex.Reset();
	PlayerIndexFromGamepadProfileIndex.Reset();

	for (int KeyboardProfileIndex = 0; KeyboardProfileIndex < LocalMultiplayerSettings->GetNbKeyboardProfiles(); ++KeyboardProfileIndex)
	{
		const int PlayerIndex = AssignNewPlayerToKeyboardProfileIndex(KeyboardProfileIndex);
		AssignKeyboardInputMapping(PlayerIndex, KeyboardProfileIndex, MappingType);
	}

	for (int DeviceID = 0; DeviceID < LocalMultiplayerSettings->NbMaxGamepads; ++DeviceID)
	{
		const int PlayerIndex = AssignNewPlayerToGamepadDeviceID(DeviceID);
		AssignGamepadInputMapping(PlayerIndex, MappingType);
	}
}

int ULocalMultiplayerSubsystem::GetAssignedPlayerIndexFromKeyboardProfileIndex(int KeyboardProfileIndex) const
{
	if (const int* Found = PlayerIndexFromKeyboardProfileIndex.Find(KeyboardProfileIndex))
	{
		return *Found;
	}
	
	return -1;
}

int ULocalMultiplayerSubsystem::GetAssignedPlayerIndexFromGamepadDeviceID(int DeviceID) const
{
	if (const int* Found = PlayerIndexFromGamepadProfileIndex.Find(DeviceID))
	{
		return *Found;
	}

	return -1;
}

int ULocalMultiplayerSubsystem::AssignNewPlayerToKeyboardProfileIndex(int KeyboardProfileIndex)
{
	const int FoundPlayerIndex = GetAssignedPlayerIndexFromKeyboardProfileIndex(KeyboardProfileIndex);
	if (FoundPlayerIndex != -1)
	{
		return FoundPlayerIndex;
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return -1;
	}

	if (LastAssignedPlayerIndex >= GameInstance->GetLocalPlayers().Num())
	{
		FString OutError;
		GameInstance->CreateLocalPlayer(LastAssignedPlayerIndex, OutError, true);
	}

	const int AssignedPlayerIndex = LastAssignedPlayerIndex++;
	PlayerIndexFromKeyboardProfileIndex.Add(KeyboardProfileIndex, AssignedPlayerIndex);
	return AssignedPlayerIndex;
}

int ULocalMultiplayerSubsystem::AssignNewPlayerToGamepadDeviceID(int DeviceID)
{
	const int FoundPlayerIndex = GetAssignedPlayerIndexFromGamepadDeviceID(DeviceID);
	if (FoundPlayerIndex != -1)
	{
		return FoundPlayerIndex;
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return -1;
	}

	if (LastAssignedPlayerIndex >= GameInstance->GetLocalPlayers().Num())
	{
		FString OutError;
		GameInstance->CreateLocalPlayer(LastAssignedPlayerIndex, OutError, true);
	}

	const int AssignedPlayerIndex = LastAssignedPlayerIndex++;
	PlayerIndexFromGamepadProfileIndex.Add(DeviceID, AssignedPlayerIndex);
	return AssignedPlayerIndex;
}

void ULocalMultiplayerSubsystem::AssignKeyboardInputMapping(int PlayerIndex, int KeyboardProfileIndex, ELocalMultiplayerInputMappingType MappingType) const
{
	const ULocalMultiplayerSettings* LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();
	if (!LocalMultiplayerSettings)
	{
		return;
	}

	if (!LocalMultiplayerSettings->KeyboardProfilesData.IsValidIndex(KeyboardProfileIndex))
	{
		return;
	}

	const UInputMappingContext* IMC = LocalMultiplayerSettings->KeyboardProfilesData[KeyboardProfileIndex].GetIMCFromType(MappingType);
	if (!IMC)
	{
		return;
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return;
	}
	
	ULocalPlayer* LocalPlayer = GameInstance->GetLocalPlayerByIndex(PlayerIndex);
	if (!LocalPlayer)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* EIS = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		EIS->ClearAllMappings();
		EIS->AddMappingContext(const_cast<UInputMappingContext*>(IMC), 0);
	}
}

void ULocalMultiplayerSubsystem::AssignGamepadInputMapping(int PlayerIndex, ELocalMultiplayerInputMappingType MappingType) const
{
	const ULocalMultiplayerSettings* LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();
	if (!LocalMultiplayerSettings)
	{
		return;
	}

	const UInputMappingContext* IMC = LocalMultiplayerSettings->GamepadProfileData.GetIMCFromType(MappingType);
	if (!IMC)
	{
		return;
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return;
	}
	
	ULocalPlayer* LocalPlayer = GameInstance->GetLocalPlayerByIndex(PlayerIndex);
	if (!LocalPlayer)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* EIS = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		EIS->ClearAllMappings();
		EIS->AddMappingContext(const_cast<UInputMappingContext*>(IMC), 0);
	}
}

