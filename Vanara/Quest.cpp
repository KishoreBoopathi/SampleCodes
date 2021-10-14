// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest.h"
#include "LocationMarker.h"
#include "Engine/GameEngine.h"
#include "Enemy.h"
#include "MainCharacter.h"
#include "VanaraGameInstance.h"

// Sets default values
AQuest::AQuest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsQuestedAccpeted = false;
	bIsStoryQuest = false;
	bIsQuestActive = false;
	bIsQuestAvailable = false;
}

// Called when the game starts or when spawned
void AQuest::BeginPlay()
{
	Super::BeginPlay();

	//this->LocationReached.AddDynamic(this, &AQuest::CheckLocationObjective);
	this->TargetInteracted.AddDynamic(this, &AQuest::CheckObjectiveTargetInteraction);
	this->EnemyKilled.AddDynamic(this, &AQuest::CheckObjectiveEnemyKilled);

	if (!PreRequisite)
	{
		bIsQuestAvailable = true;
	}
	
}

// Called every frame
void AQuest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PreRequisite)
	{
		if (PreRequisite->bIsQuestCompleted)
		{
			bIsQuestAvailable = true;
		}
	}
}

void AQuest::SetCurrentObjective()
{
	for (int32 i = 0; i < QuestObjectives.Num(); i++)
	{
		if (!QuestObjectives[i].bIsObjectiveComplete)
		{
			CurrentObjective = QuestObjectives[i];
			return;
		}
	}
}

/*void AQuest::CheckLocationObjective(ALocationMarker* ReachedLocation)
{
	for (int32 i = 0; i < QuestObjectives.Num(); i++)
	{
		if (!QuestObjectives[i].bIsObjectiveComplete && QuestObjectives[i].Target == ReachedLocation)
		{
			QuestObjectives[i].bIsObjectiveComplete = true;
			IsQuestObjectivesCompleted();
			SetCurrentObjective();
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(*QuestObjectives[i].ObjectiveDescription.ToString()) + TEXT(" | Objective Completed...."));
			}
		}
	}
}*/

void AQuest::CheckObjectiveTargetInteraction(AActor* InteractedTarget, AMainCharacter* MainCharacter)
{
	for (int32 i = 0; i < QuestObjectives.Num(); i++)
	{
		if (!QuestObjectives[i].bIsObjectiveComplete && QuestObjectives[i].TargetClass == InteractedTarget->GetClass())
		{
			UE_LOG(LogTemp, Warning, TEXT("Passed"));
			if (QuestObjectives[i].CountToComplete != 0)
			{
				--QuestObjectives[i].CountToComplete;
			}
			QuestObjectives[i].bIsObjectiveComplete = true;
			IsQuestObjectivesCompleted(MainCharacter);
			SetCurrentObjective();
			/*if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(*QuestObjectives[i].ObjectiveDescription.ToString()) + TEXT(" | Objective Completed...."));
			}*/
		}
	}
}

void AQuest::CheckObjectiveEnemyKilled(AEnemy* KilledEnemy, AMainCharacter* MainCharacter)
{
	for (int32 i = 0; i < QuestObjectives.Num(); i++)
	{
		if (!QuestObjectives[i].bIsObjectiveComplete && QuestObjectives[i].TargetClass == KilledEnemy->GetClass())
		{
			if (!((--QuestObjectives[i].CountToComplete) <= 0))
			{
				//--QuestObjectives[i].CountToComplete;
				return;
			}
			QuestObjectives[i].bIsObjectiveComplete = true;
			IsQuestObjectivesCompleted(MainCharacter);
			SetCurrentObjective();
			/*if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(*QuestObjectives[i].ObjectiveDescription.ToString()) + TEXT(" | Objective Completed...."));
			}*/
			return;
		}
	}
}

bool AQuest::IsQuestObjectivesCompleted(AMainCharacter* MainCharacter)
{
	for (FObjectiveData Objective : QuestObjectives)
	{
		if (!Objective.bIsObjectiveComplete)
		{
			return false;
		}
	}
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(*QuestName.ToString()) + TEXT(" Quest Completed...."));
	}*/
	bIsQuestCompleted = true;

	UVanaraGameInstance* GameInstance = Cast<UVanaraGameInstance>(GetGameInstance());
	GameInstance->StoryQuestsCompletedCount += 1;

	MainCharacter->bShowHonorMeter = true;
	MainCharacter->Honor += HonorCount;

	GameInstance->GenerateNextStoryQuest();
	return true;
}