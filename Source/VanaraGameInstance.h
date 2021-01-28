// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "VanaraGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_VANARA_API UVanaraGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UVanaraGameInstance();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	bool bContainsSavedData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menus")
	bool bFromMainMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menus")
	bool bToGameScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString Subtitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString Difficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString Language;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString VideoQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString Resolution;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString ScreenType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Brightness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MasterVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MusicVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float SFXVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	FName GameLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	FName MainMenuLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	FName LoadingLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 StoryQuestsCompletedCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveData")
	TSubclassOf<class AVanaraStorageRoom> StorageRoom;

protected:
	//virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable)
	FString SetGammaValue(float GammaValue);

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame();

	UFUNCTION(BlueprintCallable)
	void GenerateNextStoryQuest();

};
