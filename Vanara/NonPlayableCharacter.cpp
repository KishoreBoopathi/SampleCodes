// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayableCharacter.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "MainCharacter.h"
#include "MainPlayerController.h"
#include "Engine/GameEngine.h"
#include "QuestLog.h"
#include "Quest.h"
#include "Templates/SubclassOf.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// Sets default values
ANonPlayableCharacter::ANonPlayableCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	InteractSphere->SetupAttachment(GetRootComponent());
	InteractSphere->InitSphereRadius(75.f);
	InteractSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	
}
	

// Called when the game starts or when spawned
void ANonPlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	AIController = Cast<AAIController>(GetController());

	InteractSphere->OnComponentBeginOverlap.AddDynamic(this, &ANonPlayableCharacter::InteractSphereOnOverlapBegin);
	InteractSphere->OnComponentEndOverlap.AddDynamic(this, &ANonPlayableCharacter::InteractSphereOnOverlapEnd);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	TArray<AActor*> AttachedActors;
	this->GetAttachedActors(AttachedActors);
	if (AttachedActors.Num() > 0)
	{
		for (auto Actor : AttachedActors)
		{
			AttachedQuest = Cast<AQuest>(AttachedActors[0]);
			/*if (AttachedQuest)
			{
				if (AttachedQuest->bIsQuestAvailable)
				{
					UE_LOG(LogTemp, Warning, TEXT("Start..."));
					ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
					UE_LOG(LogTemp, Warning, TEXT("Step 1."));
					if (PlayerCharacter)
					{
						UE_LOG(LogTemp, Warning, TEXT("Step 2."));
						AMainCharacter* MainCharacter = Cast<AMainCharacter>(PlayerCharacter);
						if (MainCharacter)
						{
							UE_LOG(LogTemp, Warning, TEXT("Step 3."));
							if (MainCharacter->MainPlayerController)
							{
								UE_LOG(LogTemp, Warning, TEXT("Step 4."));
								MainCharacter->MainPlayerController->DisplayQuestMarker();

							}
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Too Soon..."));
					}
				}
			}*/
		}
	}

}

// Called every frame
void ANonPlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANonPlayableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANonPlayableCharacter::InteractSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			if (MainCharacter->MainPlayerController)
			{
				FVector ActiveOverlappingCharacterLocation = this->GetActorLocation();
				MainCharacter->MainPlayerController->InteractingLocation = ActiveOverlappingCharacterLocation;
				MainCharacter->MainPlayerController->DisplayInteractMessage();
				MainCharacter->SetActiveOverlappingCharacter(this);
			}
			
		}
	}
}

void ANonPlayableCharacter::InteractSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			if (MainCharacter->MainPlayerController)
			{
				MainCharacter->MainPlayerController->RemoveInteractMessage();
				MainCharacter->MainPlayerController->RemoveHelpMessage();
				MainCharacter->MainPlayerController->RemoveThankingMessage();
				MainCharacter->MainPlayerController->RemoveQuestDialogue();
				MainCharacter->MainPlayerController->RemoveGeneralDialogue();
				MainCharacter->SetActiveOverlappingCharacter(nullptr);
				MainCharacter->RemoveActiveOverlappingQuest();
				MainCharacter->bIsQuestDialogueWidgetCreated = false;
			}
		}
	}
}

void ANonPlayableCharacter::Interact_Implementation(AMainCharacter* MainCharacter)
{
	UQuestLog* QuestLog = MainCharacter->QuestLog;
	for (AQuest* Quest : QuestLog->Quests)
	{
		if (!Quest->bIsQuestCompleted)
			Quest->TargetInteracted.Broadcast(Cast<AActor>(this), MainCharacter);

		if (Quest->bIsQuestCompleted)
			QuestLog->SetActiveQuest(nullptr);
	}
	QuestLog->SetCurrentObjectiveDescription();
}