// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacter.h"
#include "Animation/AnimEnums.h"
#include "Containers/EnumAsByte.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			MainCharacter = Cast<AMainCharacter>(Pawn);
		}
	}
}

void UMainAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

	if (Pawn)
	{
		if (MainCharacter == nullptr)
		{
			MainCharacter = Cast<AMainCharacter>(Pawn);
		}
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
		bAttacking = MainCharacter->GetAttackingStatus();
		//this->SetRootMotionMode(ERootMotionMode::RootMotionFromEverything;

		if (MainCharacter)
		{
			if (MainCharacter->CharacterStatus == ECharacterStatus::ECS_Fighting)
			{
				FVector YDirection = MainCharacter->GetActorRightVector();
				FVector XDirection = MainCharacter->GetActorForwardVector();
				FVector Velocity = MainCharacter->GetVelocity();
				RightLeft_Speed = FVector::DotProduct(YDirection, Velocity);
				FrontBack_Speed = FVector::DotProduct(XDirection, Velocity);

				this->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
			}
		}
	}
}
