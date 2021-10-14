// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "VanaraSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FPlayerStats
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Health;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Stamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Honor;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxHonor;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float FoodCount;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float DrinkCount;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FVector Location;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString WeaponName;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 StoryQuestCompletedCount;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	TArray<class AQuest*> Quests;
};
/**
 * 
 */
UCLASS()
class PROJECT_VANARA_API UVanaraSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UVanaraSaveGame();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SlotData")
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = "SlotData")
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FPlayerStats PlayerStats;

protected:

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SlotData")
	bool bContainsSaveData;
};
