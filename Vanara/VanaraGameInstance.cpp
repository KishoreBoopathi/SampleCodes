// Fill out your copyright notice in the Description page of Project Settings.


#include "VanaraGameInstance.h"
#include "MainCharacter.h"
#include "VanaraSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon_Gada.h"
#include "VanaraStorageRoom.h"
#include "QuestLog.h"
#include "Quest.h"

UVanaraGameInstance::UVanaraGameInstance()
{
	GameLevelName = "VanaraMap";
	MainMenuLevelName = "MainMenu";
	LoadingLevelName = "LoadingScreen";

	bContainsSavedData = false;
	bFromMainMenu = false;
	bToGameScreen = true;

	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	Subtitle = "OFF";
	Difficulty = "Medium";
	Language = "English";
	VideoQuality = "Epic";
	Resolution = "1920x1080";
	ScreenType = "Full Screen";

	Brightness = 2.2f;
	MasterVolume = 0.75f;
	MusicVolume = 0.75f;
	SFXVolume = 0.75f;

	StoryQuestsCompletedCount = 0;
}



FString UVanaraGameInstance::SetGammaValue(float GammaValue)
{
	Brightness = GammaValue;
	FString Gamma = "Gamma ";
	Gamma += FString::SanitizeFloat(GammaValue);
	return Gamma;
}

void UVanaraGameInstance::SaveGame()
{
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	UVanaraSaveGame* SaveGameInstance = Cast<UVanaraSaveGame>(UGameplayStatics::CreateSaveGameObject(UVanaraSaveGame::StaticClass()));

	SaveGameInstance->PlayerStats.Health = MainCharacter->Health;
	SaveGameInstance->PlayerStats.MaxHealth = MainCharacter->MaxHealth;
	SaveGameInstance->PlayerStats.Stamina = MainCharacter->Stamina;
	SaveGameInstance->PlayerStats.MaxStamina = MainCharacter->MaxStamina;
	SaveGameInstance->PlayerStats.Honor = MainCharacter->Honor;
	SaveGameInstance->PlayerStats.MaxHonor = MainCharacter->MaxHonor;
	SaveGameInstance->PlayerStats.DrinkCount = MainCharacter->PotionCount;
	SaveGameInstance->PlayerStats.FoodCount = MainCharacter->FoodCount;

	if (MainCharacter->EquippedWeapon)
	{
		SaveGameInstance->PlayerStats.WeaponName = MainCharacter->EquippedWeapon->Name;
	}

	SaveGameInstance->PlayerStats.Location = MainCharacter->GetActorLocation();
	SaveGameInstance->PlayerStats.Rotation = MainCharacter->GetActorRotation();

	SaveGameInstance->PlayerStats.StoryQuestCompletedCount = StoryQuestsCompletedCount;

	if (MainCharacter->QuestLog->Quests.Num() > 0)
	{
		for (auto EachQuest : MainCharacter->QuestLog->Quests)
		{
			AQuest* Quest = Cast<AQuest>(EachQuest);
			if (!Quest->bIsStoryQuest)
			{
				SaveGameInstance->PlayerStats.Quests.Add(Quest);
			}
		}
	}

	bContainsSavedData = UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
}

void UVanaraGameInstance::LoadGame()
{
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	UVanaraSaveGame* LoadGameInstance = Cast<UVanaraSaveGame>(UGameplayStatics::CreateSaveGameObject(UVanaraSaveGame::StaticClass()));

	LoadGameInstance = Cast<UVanaraSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	if (LoadGameInstance)
	{
		MainCharacter->Health = LoadGameInstance->PlayerStats.Health;
		MainCharacter->MaxHealth = LoadGameInstance->PlayerStats.MaxHealth;
		MainCharacter->Stamina = LoadGameInstance->PlayerStats.Stamina;
		MainCharacter->MaxStamina = LoadGameInstance->PlayerStats.MaxStamina;
		MainCharacter->Honor = LoadGameInstance->PlayerStats.Honor;
		MainCharacter->MaxHonor = LoadGameInstance->PlayerStats.MaxHonor;
		MainCharacter->PotionCount = LoadGameInstance->PlayerStats.DrinkCount;
		MainCharacter->FoodCount = LoadGameInstance->PlayerStats.FoodCount;

		StoryQuestsCompletedCount = LoadGameInstance->PlayerStats.StoryQuestCompletedCount;

		if (StorageRoom)
		{
			AVanaraStorageRoom* Storage = GetWorld()->SpawnActor<AVanaraStorageRoom>(StorageRoom);
			if (Storage)
			{
				FString WeaponName = LoadGameInstance->PlayerStats.WeaponName;

				if (Storage->WeaponMap.Contains(WeaponName))
				{
					AWeapon_Gada* WeaponToEquip = GetWorld()->SpawnActor<AWeapon_Gada>(Storage->WeaponMap[WeaponName]);
					WeaponToEquip->Equip(MainCharacter);
				}
			}
			if (Storage->StoryQuestList.Num() > 0)
			{
				for (int i = 1; i <= StoryQuestsCompletedCount; i++)
				{
					FQuestDetails* QuestDetails = Storage->StoryQuestList.Find(i);
					if (QuestDetails)
					{
						AQuest* CurrentQuest = GetWorld()->SpawnActor<AQuest>(QuestDetails->Quest);
						CurrentQuest->bIsQuestCompleted = true;
						MainCharacter->QuestLog->AddQuestToLog(CurrentQuest);
					}
				}
			}
			GenerateNextStoryQuest();
		}

		if (LoadGameInstance->PlayerStats.Quests.Num() > 0)
		{
			for (auto EachQuest : LoadGameInstance->PlayerStats.Quests)
			{

			}
		}

		MainCharacter->SetActorLocation(LoadGameInstance->PlayerStats.Location);
		MainCharacter->SetActorRotation(LoadGameInstance->PlayerStats.Rotation);

		//GenerateNextStoryQuest();

		MainCharacter->SetCharacterStatus(ECharacterStatus::ECS_Normal);
		MainCharacter->GetMesh()->bPauseAnims = false;
		MainCharacter->GetMesh()->bNoSkeletonUpdate = false;
	}
}

void UVanaraGameInstance::GenerateNextStoryQuest()
{
	AMainCharacter *MainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	AVanaraStorageRoom* StoreRoom = Cast<AVanaraStorageRoom>(StorageRoom.GetDefaultObject());
	if (StoreRoom)
	{
		if (StoreRoom->StoryQuestList.Num() > 0)
		{		
			FQuestDetails* QuestDetails = StoreRoom->StoryQuestList.Find(StoryQuestsCompletedCount + 1);
			if (QuestDetails)
			{
				AQuest* CurrentQuest = GetWorld()->SpawnActor<AQuest>(QuestDetails->Quest);
				if (StoryQuestsCompletedCount == 0)
				{
					MainCharacter->QuestLog->AddQuestToLog(CurrentQuest);
					CurrentQuest->bIsQuestedAccpeted = true;
				}
				else
				{
					AActor* QuestLocation = UGameplayStatics::GetActorOfClass(GetWorld(), QuestDetails->QuestSpawnLocation);
					CurrentQuest->AttachToActor(QuestLocation, FAttachmentTransformRules::KeepRelativeTransform);
					for (auto EachProp : QuestDetails->QuestProps)
					{
						AActor* SpawnLocationActor = UGameplayStatics::GetActorOfClass(GetWorld(), EachProp.QuestPropLocation);
						FTransform SpawnLocation = SpawnLocationActor->GetActorTransform();
						GetWorld()->SpawnActor(EachProp.QuestProp, &SpawnLocation, FActorSpawnParameters());
					}
				}
			}
		}
	}
}
