// Fill out your copyright notice in the Description page of Project Settings.


#include "Dhundhubi_Boss.h"
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

ADhundhubi_Boss::ADhundhubi_Boss()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LeftHandCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandCombatCollision"));
	LeftHandCombatCollision->SetupAttachment(GetMesh(), FName("hand_l"));

	RightHandCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandCombatCollision"));
	RightHandCombatCollision->SetupAttachment(GetMesh(), FName("hand_r"));

	RoarCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RoarCombatCollision"));
	RoarCombatCollision->SetupAttachment(GetMesh(), FName("B_Jaw"));

	bIsBoss = true;
}

void ADhundhubi_Boss::BeginPlay()
{
	Super::BeginPlay();

	LeftHandCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &ADhundhubi_Boss::LeftHandCollisionOnOverlapBegin);
	LeftHandCombatCollision->OnComponentEndOverlap.AddDynamic(this, &ADhundhubi_Boss::LeftHandCollisionOnOverlapEnd);

	LeftHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftHandCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftHandCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);


	RightHandCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &ADhundhubi_Boss::RightHandCollisionOnOverlapBegin);
	RightHandCombatCollision->OnComponentEndOverlap.AddDynamic(this, &ADhundhubi_Boss::RightHandCollisionOnOverlapEnd);

	RightHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightHandCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RoarCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &ADhundhubi_Boss::RoarCollisionOnOverlapBegin);
	RoarCombatCollision->OnComponentEndOverlap.AddDynamic(this, &ADhundhubi_Boss::RoarCollisionOnOverlapEnd);

	RoarCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RoarCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RoarCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RoarCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ADhundhubi_Boss::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::CombatSphereOnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex,  bFromSweep, SweepResult);

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
				GetWorldTimerManager().SetTimer(AttackTimer, this, &ADhundhubi_Boss::Attack, AttackTime);
			}
		}
	}
}


void ADhundhubi_Boss::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void ADhundhubi_Boss::LeftHandCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void ADhundhubi_Boss::LeftHandCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ADhundhubi_Boss::RightHandCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void ADhundhubi_Boss::RightHandCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ADhundhubi_Boss::RoarCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			if (Main->WaterBlastHitParticles)
			{
				const USkeletalMeshSocket* TipSocket = Main->GetMesh()->GetSocketByName("BlastHitSocket");
				if (TipSocket)
				{
					FVector SocketLocation = TipSocket->GetSocketLocation(Main->GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->WaterBlastHitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
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

void ADhundhubi_Boss::RoarCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ADhundhubi_Boss::ActivateLeftHandCollision()
{
	LeftHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ADhundhubi_Boss::ActivateRightHandCollision()
{
	RightHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ADhundhubi_Boss::ActivateRoarCollision()
{
	RoarCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ADhundhubi_Boss::GroundSmashAttack()
{
	if (bOverlappingCombatSphere)
	{
		if (CombatTarget)
		{
			if (CombatTarget->LightningBlastHitParticles)
			{
				const USkeletalMeshSocket* TipSocket = CombatTarget->GetMesh()->GetSocketByName("BlastHitSocket");
				if (TipSocket)
				{
					FVector SocketLocation = TipSocket->GetSocketLocation(CombatTarget->GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CombatTarget->LightningBlastHitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			if (CombatTarget->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, CombatTarget->HitSound);
			}
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(CombatTarget, Damage, AIController, this, DamageTypeClass);
			}
		}
	}
}

void ADhundhubi_Boss::DeactivateCombatCollisions()
{
	LeftHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RoarCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADhundhubi_Boss::Attack()
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
					else if(bIsInLongRange)
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
					else
					{
						AnimInstance->Montage_Play(MediumRangeCombatMontage, 1.2f);
						AnimInstance->Montage_JumpToSection(FName("Attack 1"), MediumRangeCombatMontage);
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

void ADhundhubi_Boss::AttackEnd()
{
	bAttacking = false;
	bInterpToPlayer = false;
	if (bOverlappingCombatSphere)
	{
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &ADhundhubi_Boss::Attack, AttackTime);
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

void ADhundhubi_Boss::CheckPlayerDistance()
{
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	FVector PlayerLocation = MainCharacter->GetActorLocation();
	FVector SelfLocation = this->GetActorLocation();
	float DistanceFromPlayer = UKismetMathLibrary::Subtract_VectorVector(PlayerLocation, SelfLocation).Size();

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

float ADhundhubi_Boss::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die(DamageCauser);
	}
	else if(!bAttacking)
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