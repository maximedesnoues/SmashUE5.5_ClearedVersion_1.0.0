// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LocalMultiplayerSettings.generated.h"

class UInputMappingContext;

struct FKey;

/**
 * 
 */
UENUM()
enum class ELocalMultiplayerInputMappingType
{
	Menu,
    InGame
};

USTRUCT()
struct FLocalMultiplayerProfileData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    FName ProfileName;
    
    UPROPERTY(EditAnywhere, meta=(DisplayName="IMC Menu"))
    TObjectPtr<UInputMappingContext> IMCMenu = nullptr;

    UPROPERTY(EditAnywhere, meta=(DisplayName="IMC In Game"))
    TObjectPtr<UInputMappingContext> IMCInGame = nullptr;

    UInputMappingContext* GetIMCFromType(ELocalMultiplayerInputMappingType MappingType) const;
    bool ContainsKey(const FKey& Key, ELocalMultiplayerInputMappingType MappingType) const;
};

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Local Multiplayer Settings"))
class LOCALMULTIPLAYER_API ULocalMultiplayerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category="Local Multiplayer")
	int NbMaxGamepads = 4;

    UPROPERTY(Config, EditAnywhere, Category="Local Multiplayer")
    FLocalMultiplayerProfileData GamepadProfileData;

    UPROPERTY(Config, EditAnywhere, Category="Local Multiplayer")
    TArray<FLocalMultiplayerProfileData> KeyboardProfilesData;

	int GetNbKeyboardProfiles() const;
	int FindKeyboardProfileIndexFromKey(const FKey& Key, ELocalMultiplayerInputMappingType MappingType) const;
};
