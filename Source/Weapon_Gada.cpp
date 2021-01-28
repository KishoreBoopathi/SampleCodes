// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Gada.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "MainCharacter.h"
#include "MainPlayerController.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"

AWeapon_Gada::AWeapon_Gada()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionVolume"));
	CollisionVolume->SetupAttachment(GetRootComponent());

	CombatCollision = CreateDefaultSubobject<USphereComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());
}

void AWeapon_Gada::BeginPlay()
{
	Super::BeginPlay();

	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AWeapon_Gada::CollisionVolumeOnOverlapBegin);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AWeapon_Gada::CollisionVolumeOnOverlapEnd);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon_Gada::CombatCollisionOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon_Gada::CombatCollisionOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AWeapon_Gada::CollisionVolumeOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			MainCharacter->SetActiveOverlappingItem(this);
			if (!bIsWeaponEquipped)
			{
				if (MainCharacter->MainPlayerController)
				{
					FVector ActiveOverlappingItemLocation = this->GetActorLocation();
					MainCharacter->MainPlayerController->InteractingLocation = ActiveOverlappingItemLocation;
					MainCharacter->MainPlayerController->DisplayInteractMessage();
				}
			}
		}
	}
}

void AWeapon_Gada::CollisionVolumeOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			MainCharacter->SetActiveOverlappingItem(nullptr);
			if (MainCharacter->MainPlayerController)
			{
				MainCharacter->MainPlayerController->RemoveInteractMessage();
			}
		}
	}
}

void AWeapon_Gada::CombatCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			/*if (Enemy->HitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = Mesh->GetSocketByName("WeaponSocket");
				if (WeaponSocket)
				{
					FVector SocketLocation = WeaponSocket->GetSocketLocation(Mesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}*/
			if (Enemy->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
			}
		}
	}
}

void AWeapon_Gada::CombatCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWeapon_Gada::Equip(AMainCharacter* MainCharacter)
{
	if (MainCharacter)
	{
		SetInstigator(MainCharacter->GetController());

		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		Mesh->SetSimulatePhysics(false);

		const USkeletalMeshSocket* RightHandSocket = MainCharacter->GetMesh()->GetSocketByName("RightHandWeaponSocket");
		if (RightHandSocket)
		{
			RightHandSocket->AttachActor(this, MainCharacter->GetMesh());
			//bRotate = false;
			MainCharacter->SetEquippedWeapon(this);
			MainCharacter->SetActiveOverlappingItem(nullptr);
			MainCharacter->MainPlayerController->RemoveInteractMessage();
			bIsWeaponEquipped = true;
		}
		if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);
	}
}

void AWeapon_Gada::Detach(AMainCharacter* MainCharacter)
{
	if (MainCharacter)
	{
		SetInstigator(WeaponInstigator);

		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

		FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepRelative, EDetachmentRule::KeepRelative, EDetachmentRule::KeepRelative, false);
		this->DetachFromActor(DetachmentRules);

		Mesh->SetSimulatePhysics(true);
		SetWeaponState(EWeaponState::EWS_Pickup);
		MainCharacter->SetEquippedWeapon(nullptr);
		MainCharacter->SetActiveOverlappingItem(this);
		bIsWeaponEquipped = false;

		//Add Drop Sound here
		//if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);
	}
}

void AWeapon_Gada::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	UGameplayStatics::PlaySound2D(this, SwingSound);
}

void AWeapon_Gada::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}