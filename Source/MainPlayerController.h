// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_VANARA_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> HUDOverlayAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> WPauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> WDeathMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* DeathMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WEnemyHealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* EnemyHealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WInteractMessage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* InteractMessage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WHelpMessage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HelpMessage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WThankingMessage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* ThankingMessage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WGeneralDialogue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* GeneralDialogue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WQuestMarker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* QuestMarker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WQuestLog;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* QuestLog;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WQuestDialogue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* QuestDialogue;

	class UVanaraGameInstance* GameInstance;

	FString MainMenuName = "MainMenu";
	FString GameMapName = "VanaraMap";

	bool bPauseMenuVisible;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void DisplayPauseMenu();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void RemovePauseMenu();

	void TogglePauseMenu();

	bool bEnemyHealthBarVisible;

	UFUNCTION(BlueprintCallable)
	void DisplayEnemyHealthBar();

	UFUNCTION(BlueprintCallable)
	void RemoveEnemyHealthBar();

	void DisplayInteractMessage();
	void RemoveInteractMessage();

	void DisplayHelpMessage();
	void RemoveHelpMessage();

	void DisplayThankingMessage();
	void RemoveThankingMessage();

	void DisplayGeneralDialogue();
	void RemoveGeneralDialogue();

	void CreateQuestLogWidget();

	UFUNCTION(BlueprintCallable)
	void DisplayQuestLog();

	UFUNCTION(BlueprintCallable)
	void RemoveQuestLog();

	void DisplayQuestMarker();
	void RemoveQuestMarker();

	void CreateQuestDialogueWidget();

	void DisplayQuestDialogue();
	void RemoveQuestDialogue();

	UFUNCTION(BlueprintCallable)
	void DisplayHUDOverlay();

	UFUNCTION(BlueprintCallable)
	void RemoveHUDOverlay();

	UFUNCTION(BlueprintCallable)
	void DisplayDeathMenu();

	UFUNCTION(BlueprintCallable)
	void RemoveDeathMenu();

	FVector EnemyLocation;

	FVector InteractingLocation;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
