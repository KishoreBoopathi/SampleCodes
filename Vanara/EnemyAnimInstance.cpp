// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"
#include "Vanara_Guards.h"
#include "Dhundhubi_Boss.h"
#include "Ravana_Boss.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Ravana_Boss = Cast<ARavana_Boss>(Pawn);
			Dhundhubi_Boss = Cast<ADhundhubi_Boss>(Pawn);
			VanaraGuards = Cast<AVanara_Guards>(Pawn);
			//Enemy = Cast<AEnemy>(Pawn);
		}
	}
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Ravana_Boss = Cast<ARavana_Boss>(Pawn);
			Dhundhubi_Boss = Cast<ADhundhubi_Boss>(Pawn);
			VanaraGuards = Cast<AVanara_Guards>(Pawn);
			//Enemy = Cast<AEnemy>(Pawn);
		}
	}

	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();
	}
}