// Fill out your copyright notice in the Description page of Project Settings.


#include "Vanara_Guards.h"
#include "Components/BoxComponent.h"
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

AVanara_Guards::AVanara_Guards()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LeftHandCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandCombatCollision"));
	LeftHandCombatCollision->SetupAttachment(GetMesh(), FName("hand_l"));

	RightHandCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandCombatCollision"));
	RightHandCombatCollision->SetupAttachment(GetMesh(), FName("hand_r"));

	bIsBoss = false;
}

void AVanara_Guards::BeginPlay()
{
	Super::BeginPlay();

	LeftHandCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AVanara_Guards::LeftHandCollisionOnOverlapBegin);
	LeftHandCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AVanara_Guards::LeftHandCollisionOnOverlapEnd);

	LeftHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftHandCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftHandCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightHandCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AVanara_Guards::RightHandCollisionOnOverlapBegin);
	RightHandCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AVanara_Guards::RightHandCollisionOnOverlapEnd);

	RightHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightHandCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	EquipWeaponOnStart();
}

void AVanara_Guards::EquipWeaponOnStart()
{
	//SetInstigator(AIController);

	UWorld* World = GetWorld();

	Weapon_r = World->SpawnActor<AWeapon_Enemy>(RightHandWeapon);

	Weapon_r->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Weapon_r->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Weapon_r->Mesh->SetSimulatePhysics(false);

	const USkeletalMeshSocket* RightHandSocket = this->GetMesh()->GetSocketByName("RightHandWeaponSocket");
	if (RightHandSocket)
	{
		RightHandSocket->AttachActor(Weapon_r, this->GetMesh());
	}
}

void AVanara_Guards::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				GetWorldTimerManager().SetTimer(AttackTimer, this, &AVanara_Guards::Attack, AttackTime);
			}
		}
	}
}


void AVanara_Guards::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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
				CombatTarget = nullptr;
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

void AVanara_Guards::LeftHandCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AVanara_Guards::LeftHandCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AVanara_Guards::RightHandCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AVanara_Guards::RightHandCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AVanara_Guards::ActivateLeftHandCollision()
{
	LeftHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AVanara_Guards::ActivateRightHandCollision()
{
	RightHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AVanara_Guards::ActivateWeaponCollision()
{
	Weapon_r->ActivateCollision();
}

void AVanara_Guards::DeactivateCombatCollisions()
{
	LeftHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Weapon_r->DeactivateCollision();
}

void AVanara_Guards::Attack()
{
	if (IsAlive())
	{
		if (AIController)
		{
			AIController->StopMovement();
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
			if (!bAttacking)
			{
				bAttacking = true;
				bIsAttackHeavy = false;
				bInterpToPlayer = true;
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance)
				{
					float RandomNumber = FMath::FRandRange(-1, 1);
					int32 AttackNumber = (RandomNumber <= 0) ? 1 : 2;
					switch (AttackNumber)
					{
					case 1:
						AnimInstance->Montage_Play(CloseRangeCombatMontage, 1.35f);
						AnimInstance->Montage_JumpToSection(FName("Attack 1"), CloseRangeCombatMontage);
						break;
					case 2:
						AnimInstance->Montage_Play(CloseRangeCombatMontage, 1.35f);
						AnimInstance->Montage_JumpToSection(FName("Attack 2"), CloseRangeCombatMontage);
						break;
					default:
						break;
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

void AVanara_Guards::AttackEnd()
{
	bAttacking = false;
	bInterpToPlayer = false;
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_ReadyToEngage);
	if (bOverlappingCombatSphere)
	{
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AVanara_Guards::Attack, AttackTime);
	}
	else
	{
		if (CombatTarget && (GetEnemyMovementStatus()!=EEnemyMovementStatus::EMS_Attacking))
		{
			FTimerDelegate ResponseDelegate = FTimerDelegate::CreateUObject(this, &AEnemy::MoveToTarget, CombatTarget);
			GetWorldTimerManager().SetTimer(ResponseTimer, ResponseDelegate, ResponseTime, false);
		}
	}
}

void AVanara_Guards::CheckPlayerDistance()
{
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	FVector PlayerLocation = MainCharacter->GetActorLocation();
	FVector SelfLocation = this->GetActorLocation();
	float DistanceFromPlayer = UKismetMathLibrary::Subtract_VectorVector(PlayerLocation, SelfLocation).Size();
	UE_LOG(LogTemp, Warning, TEXT("Distance From Player : %f"), DistanceFromPlayer);

	if (DistanceFromPlayer < 130.f)
	{
		bIsInCloseRange = true;
		bIsInMediumRange = false;
		bIsInLongRange = false;
	}
	else if (DistanceFromPlayer < 215.f)
	{
		bIsInCloseRange = false;
		bIsInMediumRange = true;
		bIsInLongRange = false;
	}
	else if (DistanceFromPlayer < 450.f)
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

float AVanara_Guards::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die(DamageCauser);
	}
	else
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
					AnimInstance->Montage_Play(HitReactMontage, 0.9f);
					AnimInstance->Montage_JumpToSection(FName("HitFromFront"), HitReactMontage);
					break;
				case 1:
				case 2:
					AnimInstance->Montage_Play(HitReactMontage, 0.9f);
					AnimInstance->Montage_JumpToSection(FName("HitFromRight"), HitReactMontage);
					break;
				case 3:
				case 4:
					AnimInstance->Montage_Play(HitReactMontage, 0.9f);
					AnimInstance->Montage_JumpToSection(FName("HitFromBack"), HitReactMontage);
					break;
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