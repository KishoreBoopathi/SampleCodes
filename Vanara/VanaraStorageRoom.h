// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VanaraStorageRoom.generated.h"

USTRUCT(BlueprintType)
struct FQuestPropDetails
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest | Props")
	TSubclassOf<AActor> QuestProp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest | Props")
	TSubclassOf<AActor> QuestPropLocation;
};

USTRUCT(BlueprintType)
struct FQuestDetails
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest | List")
	TSubclassOf<class AQuest> Quest;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest | List")
	TSubclassOf<AActor> QuestSpawnLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest | List")
	TArray<FQuestPropDetails> QuestProps;
};

UCLASS()
class PROJECT_VANARA_API AVanaraStorageRoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVanaraStorageRoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SaveData")
	TMap<FString, TSubclassOf<class AWeapon_Base>> WeaponMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QuestList")
	TMap<int32, FQuestDetails> StoryQuestList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QuestList")
	TMap<int32, FQuestDetails> SideQuestList;

};
