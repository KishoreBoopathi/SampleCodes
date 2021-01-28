// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Quest.generated.h"

UENUM(BlueprintType)
enum class EObjectiveType : uint8
{
	EOT_Location	UMETA(DisplayName = "Location"),
	EOT_Interact	UMETA(DisplayName = "Interact"),
	EOT_Collect		UMETA(DisplayName = "Collect"),
	EOT_Kill		UMETA(DisplayName = "kill"),

	EOT_Max			UMETA(DisplayName = "DefaultMax")
};

USTRUCT(BlueprintType)
struct FObjectiveData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Objective")
	FText ObjectiveDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Objective")
	EObjectiveType ObjectiveType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Objective")
	TSubclassOf<AActor> TargetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Objective")
	int32 CountToComplete;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Quest|Objective")
	bool bIsObjectiveComplete;
};

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLocationReached, class ALocationMarker*, Location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTargetInteracted, class AActor*, Target, class AMainCharacter*, MainCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEnemyKilled, class AEnemy*, Enemy, class AMainCharacter*, MainCharacter);

UCLASS()
class PROJECT_VANARA_API AQuest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuest();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	bool bIsStoryQuest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 StoryQuestNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText QuestDescription;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Quest")
	bool bIsQuestedAccpeted;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Quest")
	bool bIsQuestActive;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Quest")
	bool bIsQuestCompleted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TArray<FObjectiveData> QuestObjectives;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Quest")
	FObjectiveData CurrentObjective;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	float HonorCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	class AQuest* PreRequisite;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Quest")
	bool bIsQuestAvailable;

	/*UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FLocationReached LocationReached;*/

	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FTargetInteracted TargetInteracted;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FEnemyKilled EnemyKilled;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetCurrentObjective();

	/*UFUNCTION(BlueprintCallable)
	void CheckLocationObjective(ALocationMarker* ReachedLocation);*/

	UFUNCTION(BlueprintCallable)
	void CheckObjectiveTargetInteraction(AActor* InteractedTarget, AMainCharacter* MainCharacter);

	UFUNCTION(BlueprintCallable)
	void CheckObjectiveEnemyKilled(class AEnemy* KilledEnemy, AMainCharacter* MainCharacter);

	UFUNCTION(BlueprintCallable)
	bool IsQuestObjectivesCompleted(AMainCharacter* MainCharacter);
};
