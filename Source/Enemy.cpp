// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "MainCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathlibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayerController.h"
#include "Quest.h"
#include "QuestLog.h"
#include "Weapon_Base.h"
#include "Engine/GameEngine.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	EngagingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("EngagingSphere"));
	EngagingSphere->SetupAttachment(GetRootComponent());
	EngagingSphere->InitSphereRadius(600.f);
	EngagingSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	bOverlappingCombatSphere = false;

	CloseRangeAcceptanceRadius = 10.f;
	LongRangeAcceptanceRadius = 250.f;

	Health = 1.f;
	MaxHealth = 100.f;
	Damage = 10.f;

	AttackMinTime = 0.5f;
	AttackMaxTime = 2.5f;

	InterpSpeed = 15.f;
	bInterpToPlayer = false;

	ResponseTime = 0.7f;

	bIsAttackHeavy = false;

	bIsInCloseRange = false;
	bIsInMediumRange = false;
	bIsInLongRange = true;

	DeathDelay = 10.0f;

	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	AIController = Cast<AAIController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	EngagingSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::EngagingSphereOnOverlapBegin);
	EngagingSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::EngagingSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterpToPlayer && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FRotator AEnemy::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && IsAlive())
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			MoveToTarget(MainCharacter);
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			if (MainCharacter->CombatTarget == this)
			{
				MainCharacter->SetCombatTarget(nullptr);
			}
			MainCharacter->SetHasCombatTarget(false);
			MainCharacter->UpdateCombatTarget();
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			CombatTarget = nullptr;
			if (AIController)
			{
				AIController->StopMovement();
			}
		}
	}
}

void AEnemy::EngagingSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{

		}
	}
}

void AEnemy::EngagingSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}


void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AEnemy::MoveToTarget(AMainCharacter* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MovingToTarget);
	if (AIController && GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Attacking)
	{
		float AcceptanceRadius = CloseRangeAcceptanceRadius;
		if (bIsBoss)
		{
			AcceptanceRadius = FMath::FRandRange(-2, 1);
			if (AcceptanceRadius <= 0)
			{
				AcceptanceRadius = CloseRangeAcceptanceRadius;
				bIsInLongRange = false;
			}
			else
			{
				AcceptanceRadius = LongRangeAcceptanceRadius;
				bIsInLongRange = true;
			}
		}
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

		FNavPathSharedPtr NavPath;
		AIController->MoveTo(MoveRequest, &NavPath);

		/*TArray<FNavPathPoint> NavPathPoint = NavPath->GetPathPoints();
		for (auto Point : NavPathPoint)
		{
			FVector Location = Point.Location;

			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Red, 15.f, 0.5f);
		}*/
	}
}

void AEnemy::Die(AActor* Causer)
{
	//CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EngagingSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bool bIsEnemyReadyToDie = false;
	AWeapon_Base* Weapon = Cast<AWeapon_Base>(Causer);
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(Causer);
	if (Weapon || MainCharacter)
	{
		if (Weapon)
		{
			AActor* Weilder = Weapon->GetAttachParentActor();
			MainCharacter = Cast<AMainCharacter>(Weilder);
		}
		if (MainCharacter)
		{
			MainCharacter->UpdateCombatTarget();
			UQuestLog* QuestLog = MainCharacter->QuestLog;
			if (QuestLog)
			{
				for (AQuest* Quest : QuestLog->Quests)
				{
					if (!Quest->bIsQuestCompleted)
					{
						Quest->EnemyKilled.Broadcast(this, MainCharacter);
					}
					if (Quest->bIsQuestCompleted)
					{
						QuestLog->SetActiveQuest(nullptr);
						QuestLog->SelectedQuest = nullptr;
					}
				}
				QuestLog->SetCurrentObjectiveDescription();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No Active QuestLog...."));
			}
		}
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CloseRangeCombatMontage, 1.5f);
		AnimInstance->Montage_JumpToSection(FName("Death"), CloseRangeCombatMontage);
	}
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);

	//bAttacking = false;
}

void AEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

bool AEnemy::IsAlive()
{
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::Disappear()
{
	Destroy();
}