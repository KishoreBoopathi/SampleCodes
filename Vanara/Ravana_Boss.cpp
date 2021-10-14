// Fill out your copyright notice in the Description page of Project Settings.


#include "Ravana_Boss.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainCharacter.h"
#include "Kismet/KismetMathlibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MainPlayerController.h"
#include "AIController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Weapon_Base.h"
#include "Weapon_Gada.h"
#include "Weapon_Enemy.h"
#include "Engine/GameEngine.h"

ARavana_Boss::ARavana_Boss()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RightLegCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightLegCombatCollision"));
	RightLegCombatCollision->SetupAttachment(GetMesh(), FName("foot_r"));

	bIsBoss = true;
}

void ARavana_Boss::BeginPlay()
{
	Super::BeginPlay();

	RightLegCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &ARavana_Boss::RightLegCollisionOnOverlapBegin);
	RightLegCombatCollision->OnComponentEndOverlap.AddDynamic(this, &ARavana_Boss::RightLegCollisionOnOverlapEnd);

	RightLegCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightLegCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightLegCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightLegCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	EquipWeaponOnStart();
}

void ARavana_Boss::EquipWeaponOnStart()
{
	//SetInstigator(AIController);

	UWorld* World = GetWorld();

	Weapon_l = World->SpawnActor<AWeapon_Enemy>(LeftHandWeapon);

	Weapon_l->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Weapon_l->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Weapon_l->Mesh->SetSimulatePhysics(false);

	const USkeletalMeshSocket* RightHandSocket = this->GetMesh()->GetSocketByName("hand_rSocket");
	if (RightHandSocket)
	{
		RightHandSocket->AttachActor(Weapon_l, this->GetMesh());
	}
}

void ARavana_Boss::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::CombatSphereOnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		{
			if (MainCharacter && IsAlive())
			{
				MainCharacter->SetCombatTarget(this);
				MainCharacter->SetHasCombatTarget(true);
				MainCharacter->UpdateCombatTarget();
				CombatTarget = MainCharacter;
				bOverlappingCombatSphere = true;
				float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
				GetWorldTimerManager().SetTimer(AttackTimer, this, &ARavana_Boss::Attack, AttackTime);
			}
		}
	}
}


void ARavana_Boss::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::CombatSphereOnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (OtherActor && OtherComp)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		{
			if (MainCharacter)
			{
				bOverlappingCombatSphere = false;
				FTimerDelegate ResponseDelegate = FTimerDelegate::CreateUObject(this, &AEnemy::MoveToTarget, MainCharacter);
				GetWorldTimerManager().SetTimer(ResponseTimer, ResponseDelegate, ResponseTime, false);
				if (MainCharacter->CombatTarget == this)
				{
					MainCharacter->SetCombatTarget(nullptr);
					MainCharacter->bHasCombatTarget = false;
					MainCharacter->UpdateCombatTarget();
				}
				if (MainCharacter->MainPlayerController)
				{
					USkeletalMeshComponent* MainCharacterMesh = Cast<USkeletalMeshComponent>(OtherComp);
					if (MainCharacterMesh)
						MainCharacter->MainPlayerController->RemoveEnemyHealthBar();
				}

				GetWorldTimerManager().ClearTimer(AttackTimer);
			}
		}
	}
}

void ARavana_Boss::RightLegCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			/*if (Main->HitParticles)
			{
				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
				if (TipSocket)
				{
					FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}*/
			if (Main->HitSound)
			{

				UGameplayStatics::PlaySound2D(this, Main->HitSound);
			}
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
			}
		}
	}
}

void ARavana_Boss::RightLegCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ARavana_Boss::ActivateRightLegCollision()
{
	RightLegCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ARavana_Boss::ActivateWeaponCollision()
{
	Weapon_l->ActivateCollision();
}

void ARavana_Boss::DeactivateCombatCollisions()
{
	RightLegCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Weapon_l->DeactivateCollision();
}

void ARavana_Boss::Attack()
{
	if (IsAlive())
	{
		if (AIController)
		{
			AIController->StopMovement();
			if (!bAttacking)
			{
				CheckPlayerDistance();
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
				bAttacking = true;
				bIsAttackHeavy = true;
				bInterpToPlayer = true;
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance)
				{
					if (bIsInCloseRange)
					{
						AnimInstance->Montage_Play(CloseRangeCombatMontage, 1.15f);
						AnimInstance->Montage_JumpToSection(FName("Attack 1"), CloseRangeCombatMontage);
					}
					else if (bIsInMediumRange)
					{
						float RandomNumber = FMath::FRandRange(-1, 1);
						int32 AttackNumber = (RandomNumber <= 0) ? 1 : 2;
						switch (AttackNumber)
						{
						case 1:
							AnimInstance->Montage_Play(MediumRangeCombatMontage, 1.1f);
							AnimInstance->Montage_JumpToSection(FName("Attack 1"), MediumRangeCombatMontage);
							break;
						case 2:
							AnimInstance->Montage_Play(MediumRangeCombatMontage, 1.1f);
							AnimInstance->Montage_JumpToSection(FName("Attack 2"), MediumRangeCombatMontage);
							break;
						default:
							break;
						}
					}
					else if (bIsInLongRange)
					{
						float RandomNumber = FMath::FRandRange(-1, 1);
						int32 AttackNumber = (RandomNumber <= 0) ? 1 : 2;
						switch (AttackNumber)
						{
						case 1:
							AnimInstance->Montage_Play(LongRangeCombatMontage, 1.2f);
							AnimInstance->Montage_JumpToSection(FName("Attack 1"), LongRangeCombatMontage);
							break;
						case 2:
							AnimInstance->Montage_Play(LongRangeCombatMontage, 1.2f);
							AnimInstance->Montage_JumpToSection(FName("Attack 2"), LongRangeCombatMontage);
							break;
						default:
							break;
						}
					}
				}
				if (SwingSound)
				{
					UGameplayStatics::PlaySound2D(this, SwingSound);
				}
			}
		}
	}
}

void ARavana_Boss::AttackEnd()
{
	bAttacking = false;
	bInterpToPlayer = false;
	if (bOverlappingCombatSphere)
	{
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &ARavana_Boss::Attack, AttackTime);
	}
	else
	{
		if (CombatTarget)
		{
			FTimerDelegate ResponseDelegate = FTimerDelegate::CreateUObject(this, &AEnemy::MoveToTarget, CombatTarget);
			GetWorldTimerManager().SetTimer(ResponseTimer, ResponseDelegate, ResponseTime, false);
		}
	}
}

void ARavana_Boss::CheckPlayerDistance()
{
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	FVector PlayerLocation = MainCharacter->GetActorLocation();
	FVector SelfLocation = this->GetActorLocation();
	float DistanceFromPlayer = UKismetMathLibrary::Subtract_VectorVector(PlayerLocation, SelfLocation).Size();

	if (DistanceFromPlayer < 90.f)
	{
		bIsInCloseRange = true;
		bIsInMediumRange = false;
		bIsInLongRange = false;
	}
	else if (DistanceFromPlayer < 140.f)
	{
		bIsInCloseRange = false;
		bIsInMediumRange = true;
		bIsInLongRange = false;
	}
	else if (DistanceFromPlayer < 200.f)
	{
		bIsInCloseRange = false;
		bIsInMediumRange = false;
		bIsInLongRange = true;
	}
	else if (GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_MovingToTarget)
	{
		MoveToTarget(MainCharacter);
	}
}

float ARavana_Boss::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die(DamageCauser);
		Weapon_l->Detach(this);
	}
	else if (!bAttacking)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(DamageCauser);
		AWeapon_Gada* Weapon = Cast<AWeapon_Gada>(DamageCauser);
		if (MainCharacter || Weapon)
		{
			if (Weapon)
			{
				AActor* Weilder = Weapon->GetAttachParentActor();
				MainCharacter = Cast<AMainCharacter>(Weilder);
			}
			FRotator DeltaRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), MainCharacter->GetActorLocation());
			if (DeltaRotation.Yaw < 0)
				DeltaRotation.Yaw += 360;

			FRotator SelfRotation = GetActorRotation();
			if (SelfRotation.Yaw < 0)
				SelfRotation.Yaw += 360;

			float DeltaYawRotation = DeltaRotation.Yaw - SelfRotation.Yaw;

			int32 HitFromDirection = FMath::RoundToInt(DeltaYawRotation / 45.f);
			if (HitFromDirection < 0)
				HitFromDirection += 7;

			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				switch (HitFromDirection)
				{
				case 0:
				case 7:
				case 1:
				case 2:
					AnimInstance->Montage_Play(HitReactMontage, 0.9f);
					AnimInstance->Montage_JumpToSection(FName("HitFromRight"), HitReactMontage);
					break;
				case 3:
				case 4:
				case 5:
				case 6:
					AnimInstance->Montage_Play(HitReactMontage, 0.9f);
					AnimInstance->Montage_JumpToSection(FName("HitFromLeft"), HitReactMontage);
					break;
				default:
					break;
				}
			}
		}
		Health -= DamageAmount;
	}
	return DamageAmount;
}