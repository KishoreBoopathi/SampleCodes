// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Vanara_Guards.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_VANARA_API AVanara_Guards : public AEnemy
{
	GENERATED_BODY()

public:

	AVanara_Guards();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | Combat")
	class UBoxComponent* LeftHandCombatCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | Combat")
	UBoxComponent* RightHandCombatCollision;

	UPROPERTY(EditDefaultsOnly, Category = "AI | Weapon")
	TSubclassOf<class AWeapon_Enemy> RightHandWeapon;

	class AWeapon_Enemy* Weapon_r;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:


	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void EquipWeaponOnStart();

	UFUNCTION(BlueprintCallable)
	void ActivateLeftHandCollision();

	UFUNCTION(BlueprintCallable)
	void ActivateRightHandCollision();

	UFUNCTION(BlueprintCallable)
	void ActivateWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCombatCollisions();

	UFUNCTION()
	void LeftHandCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void LeftHandCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void RightHandCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void RightHandCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

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
