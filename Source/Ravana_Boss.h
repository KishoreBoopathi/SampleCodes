// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Ravana_Boss.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_VANARA_API ARavana_Boss : public AEnemy
{
	GENERATED_BODY()

public:

	ARavana_Boss();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* MediumRangeCombatMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* LongRangeCombatMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | Combat")
	class UBoxComponent* RightLegCombatCollision;

	UPROPERTY(EditDefaultsOnly, Category = "AI | Weapon")
	TSubclassOf<class AWeapon_Enemy> LeftHandWeapon;

	class AWeapon_Enemy* Weapon_l;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	void RightLegCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void RightLegCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void EquipWeaponOnStart();

	UFUNCTION(BlueprintCallable)
	void ActivateRightLegCollision();

	UFUNCTION(BlueprintCallable)
	void ActivateWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCombatCollisions();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttacking;

	UFUNCTION()
	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION()
	void CheckPlayerDistance();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};
