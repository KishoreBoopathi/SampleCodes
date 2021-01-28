// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "VanaraGameInstance.h"


void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FVector2D Alignment(0.f, 0.f);
	
	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthBar)
		{
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
		EnemyHealthBar->SetAlignmentInViewport(Alignment);
	}

	if (WPauseMenu)
	{
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
		if (PauseMenu)
		{
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (WDeathMenu)
	{
		DeathMenu = CreateWidget<UUserWidget>(this, WDeathMenu);
		if (DeathMenu)
		{
			DeathMenu->AddToViewport();
			DeathMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (WInteractMessage)
	{
		InteractMessage = CreateWidget<UUserWidget>(this, WInteractMessage);
		if (InteractMessage)
		{
			InteractMessage->AddToViewport();
			InteractMessage->SetVisibility(ESlateVisibility::Hidden);
		}
		InteractMessage->SetAlignmentInViewport(Alignment);
	}

	if (WHelpMessage)
	{
		HelpMessage = CreateWidget<UUserWidget>(this, WHelpMessage);
		if (HelpMessage)
		{
			HelpMessage->AddToViewport();
			HelpMessage->SetVisibility(ESlateVisibility::Hidden);
		}
		HelpMessage->SetAlignmentInViewport(Alignment);
	}

	if (WThankingMessage)
	{
		ThankingMessage = CreateWidget<UUserWidget>(this, WThankingMessage);
		if (ThankingMessage)
		{
			ThankingMessage->AddToViewport();
			ThankingMessage->SetVisibility(ESlateVisibility::Hidden);
		}
		ThankingMessage->SetAlignmentInViewport(Alignment);	
	}

	if (WGeneralDialogue)
	{
		GeneralDialogue = CreateWidget<UUserWidget>(this, WGeneralDialogue);
		if (GeneralDialogue)
		{
			GeneralDialogue->AddToViewport();
			GeneralDialogue->SetVisibility(ESlateVisibility::Hidden);
		}
		GeneralDialogue->SetAlignmentInViewport(Alignment);
	}

	if (WQuestMarker)
	{
		QuestMarker = CreateWidget<UUserWidget>(this, WQuestMarker);
		if (QuestMarker)
		{
			QuestMarker->AddToViewport();
			QuestMarker->SetVisibility(ESlateVisibility::Hidden);
		}
		QuestMarker->SetAlignmentInViewport(Alignment);
	}

}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyHealthBar)
	{
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		PositionInViewport.Y -= 85.f;

		FVector2D SizeInViewport(300.f, 20.f);

		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}

	if (PauseMenu)
	{
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		PositionInViewport.Y -= 85.f;

		FVector2D SizeInViewport(300.f, 20.f);

		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}

	if (InteractMessage)
	{
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(InteractingLocation, PositionInViewport);
		PositionInViewport.operator-= (FVector2D(70.f, 120.f)); 

		FVector2D SizeInViewport(300.f, 20.f);

		InteractMessage->SetPositionInViewport(PositionInViewport);
		InteractMessage->SetDesiredSizeInViewport(SizeInViewport);
	}

	if (HelpMessage)
	{
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(InteractingLocation, PositionInViewport);
		PositionInViewport.Y -= 85.f;

		FVector2D SizeInViewport(300.f, 20.f);

		HelpMessage->SetPositionInViewport(PositionInViewport);
		HelpMessage->SetDesiredSizeInViewport(SizeInViewport);
	}

	if (ThankingMessage)
	{
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(InteractingLocation, PositionInViewport);
		PositionInViewport.Y -= 85.f;

		FVector2D SizeInViewport(300.f, 20.f);

		ThankingMessage->SetPositionInViewport(PositionInViewport);
		ThankingMessage->SetDesiredSizeInViewport(SizeInViewport);
	}

	if (GeneralDialogue)
	{
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(InteractingLocation, PositionInViewport);
		PositionInViewport.Y -= 85.f;

		FVector2D SizeInViewport(300.f, 20.f);

		GeneralDialogue->SetPositionInViewport(PositionInViewport);
		GeneralDialogue->SetDesiredSizeInViewport(SizeInViewport);
	}

	if (QuestMarker)
	{
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(InteractingLocation, PositionInViewport);
		PositionInViewport.operator-= (FVector2D(70.f, 120.f));

		FVector2D SizeInViewport(300.f, 20.f);

		QuestMarker->SetPositionInViewport(PositionInViewport);
		QuestMarker->SetDesiredSizeInViewport(SizeInViewport);
	}

}

void AMainPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::DisplayPauseMenu_Implementation()
{
	if (PauseMenu)
	{
		bPauseMenuVisible = true;
		RemoveHUDOverlay();
		PauseMenu->SetVisibility(ESlateVisibility::Visible);

		FInputModeGameAndUI InputModeGameAndUI;
		SetInputMode(InputModeGameAndUI);

		bShowMouseCursor = true;
	}
}

void AMainPlayerController::RemovePauseMenu_Implementation()
{
	if (PauseMenu)
	{
		GameInstance = Cast<UVanaraGameInstance>(GetGameInstance());

		if (GameInstance)
		{
			if (GameInstance->bToGameScreen)
			{
				DisplayHUDOverlay();
				FInputModeGameOnly InputModeGameOnly;
				SetInputMode(InputModeGameOnly);
				bShowMouseCursor = false;
			}	
		}
		bPauseMenuVisible = false;
	}
}

void AMainPlayerController::TogglePauseMenu()
{
	if (bPauseMenuVisible)
	{
		RemovePauseMenu();
		UGameplayStatics::SetGamePaused(GetWorld(), false);
	}
	else
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		DisplayPauseMenu();
	}
}

void AMainPlayerController::DisplayInteractMessage()
{
	if (InteractMessage)
	{
		InteractMessage->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveInteractMessage()
{
	if (InteractMessage)
	{
		InteractMessage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::DisplayHelpMessage()
{
	if (HelpMessage)
	{
		HelpMessage->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveHelpMessage()
{
	if (HelpMessage)
	{
		HelpMessage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::DisplayThankingMessage()
{
	if (ThankingMessage)
	{
		ThankingMessage->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveThankingMessage()
{
	if (ThankingMessage)
	{
		ThankingMessage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::DisplayGeneralDialogue()
{
	if (GeneralDialogue)
	{
		GeneralDialogue->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveGeneralDialogue()
{
	if (GeneralDialogue)
	{
		GeneralDialogue->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::DisplayQuestMarker()
{
	if (QuestMarker)
	{
		QuestMarker->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveQuestMarker()
{
	if (QuestMarker)
	{
		QuestMarker->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::CreateQuestLogWidget()
{
	if (WQuestLog)
	{
		QuestLog = CreateWidget<UUserWidget>(this, WQuestLog);
		if (QuestLog)
		{
			QuestLog->AddToViewport();
			QuestLog->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void AMainPlayerController::DisplayQuestLog()
{
	if (QuestLog)
	{
		QuestLog->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveQuestLog()
{
	if (QuestLog)
	{
		QuestLog->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::CreateQuestDialogueWidget()
{
	if (WQuestDialogue)
	{
		QuestDialogue = CreateWidget<UUserWidget>(this, WQuestDialogue);
		if (QuestDialogue)
		{
			QuestDialogue->AddToViewport();
			QuestDialogue->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void AMainPlayerController::DisplayQuestDialogue()
{
	if (QuestDialogue)
	{
		QuestDialogue->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveQuestDialogue()
{
	if (QuestDialogue)
	{
		QuestDialogue->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::DisplayHUDOverlay()
{
	if (HUDOverlay)
	{
		HUDOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveHUDOverlay()
{
	if (HUDOverlay)
	{
		HUDOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::DisplayDeathMenu()
{
	if (DeathMenu)
	{
		DeathMenu->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveDeathMenu()
{
	if (DeathMenu)
	{
		DeathMenu->SetVisibility(ESlateVisibility::Hidden);
	}
}