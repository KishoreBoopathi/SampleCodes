
// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestLog.h"
#include "Quest.h"
#include "Engine/GameEngine.h"

// Sets default values for this component's properties
UQuestLog::UQuestLog()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UQuestLog::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UQuestLog::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UQuestLog::AddQuestToLog(AQuest* QuestToAdd)
{
	if (QuestToAdd)
	{
		Quests.Add(QuestToAdd);
		if (!QuestToAdd->bIsQuestCompleted)
		{
			SetActiveQuest(QuestToAdd);
			SelectedQuest = QuestToAdd;
		} 
	}	
}

void UQuestLog::SetActiveQuest(AQuest* QuestToSetAsActive)
{
	if (ActiveQuest)
	{
		ActiveQuest->bIsQuestActive = false;
	}
	ActiveQuest = QuestToSetAsActive;
	SetCurrentObjectiveDescription();
}

void UQuestLog::SetCurrentObjectiveDescription()
{
	if (ActiveQuest)
	{
		if (!ActiveQuest->bIsQuestCompleted)
		{
			CurrentObjectiveDescription = ActiveQuest->CurrentObjective.ObjectiveDescription;
		}
	}
	else
	{
		CurrentObjectiveDescription.GetEmpty();
	}
}
