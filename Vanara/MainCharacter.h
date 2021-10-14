// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterStatus : uint8
{
	ECS_Normal		UMETA(DisplayName = "Normal"),
	ECS_Sprinting	UMETA(DisplayName = "Sprinting"),
	ECS_Fighting	UMETA(DisplayName = "Fighting"),
	ECS_Dead		UMETA(DisplayName = "Dead"),

	EMS_Max			UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{	
	ESS_Normal				UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum		UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted			UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_Max					UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class PROJECT_VANARA_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TSubclassOf<class AVanaraStorageRoom> StorageRoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest|Log")
	class UQuestLog* QuestLog;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Items")
	FVector ActiveOverlappingItemLocation;

	TArray<FVector> PickupLocations;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items)
	class AWeapon_Gada* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	class AActor* ActiveOverlappingItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	class ANonPlayableCharacter* ActiveOverlappingCharacter;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	ECharacterStatus CharacterStatus;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintingSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float FightingStateSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinStaminaStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bFightMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Socket, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* LeftHandCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Socket, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightHandCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Socket, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightLegCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Socket, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* HeadCollisionCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* MinimapBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneCaptureComponent2D* MinimapCapture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* IconSprite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float FightingStateTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameInstance")
	class UVanaraGameInstance* GameInstance;

	//Player Stats

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Health;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Stamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxHonor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Honor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	int32 MaxFoodCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	int32 FoodCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	int32 HealthGainPerFoodIntake;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	int32 StaminaGainPerPotionIntake;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	int32 MaxPotionCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	int32 PotionCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerStats")
	bool bIsActiveItemFood;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveData")
	bool bContainsSaveData;

	void DecreamentHealth(float Amount);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	bool IncreamentFood();

	UFUNCTION()
	bool IncreamentPotion();

	UFUNCTION()
	void IncreamentHealth();

	UFUNCTION()
	void IncreamentStamina();

	void Die();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* WaterBlastHitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* LightningBlastHitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	float InterpSpeed;
	bool bInterpToEnemy;

	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	FRotator GetLookAtRotationYaw(FVector Target);

	virtual void Jump() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	class AQuest* ActiveOverlappingQuest;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	bool bIsQuestDialogueWidgetCreated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Honor")
	bool bShowHonorMeter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Honor")
	float HonorBarTimer;

	bool bIsDropKeyDown;
	float DropKeyDownTimer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool FreeToMove(float Value);

	void MoveFrontBack(float Input);
	void MoveRightLeft(float Input);

	bool bMovingFrontBack;
	bool bMovingRightLeft;

	void TurnLeftRight(float Input);
	void LookUpDown(float Input);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	bool bShiftKeyDown;

	void ShiftKeyDown();
	void ShiftKeyUp();

	bool bInteractKeyDown;

	void InteractKeyDown();
	void InteractKeyUp();

	void QuestLogKeyDown();
	void QuestLogKeyUp();

	void AcceptKeyDown();
	void AcceptKeyUp();

	void HonorMeterKeyDown();
	void HonorMeterKeyUp();

	bool bLightAttackKeyDown;
	bool bHeavyAttackKeyDown;

	void LightAttackKeyDown();
	void LightAttackKeyUp();

	void HeavyAttackKeyDown();
	void HeavyAttackKeyUp();

	void DropKeyDown();
	void DropKeyUp();

	void TabKeyDown();
	void TabKeyUp();

	void UseKeyDown();
	void UseKeyUp();

	bool bEscapeKeyDown;

	void EscapeKeyDown();
	void EscapeKeyUp();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bStartComboTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 ComboKeyCount;

	FTimerHandle ComboTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackDelay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 LightAttackKeyTimesClicked;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 HeavyAttackKeyTimesClicked;

	FORCEINLINE bool GetAttackingStatus() { return bAttacking; }

	void AttackType();

	void Attack(UAnimMontage* CurrentMontage, float Attack1_Speed, float Attack2_Speed, float Attack3_Speed);

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsHeavyAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	int32 CurrentAttackSelection;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	UFUNCTION(BlueprintCallable)
	void ToggleActiveConsumable();

	void SetCharacterStatus(ECharacterStatus Status);
	
	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	void SetEquippedWeapon(class AWeapon_Gada* WeaponToSet);
	FORCEINLINE AWeapon_Gada* GetEquippedWeapon() { return EquippedWeapon; }

	FORCEINLINE void SetActiveOverlappingItem(AActor* Item) { ActiveOverlappingItem = Item; }

	FORCEINLINE void SetActiveOverlappingCharacter(ANonPlayableCharacter* Character) { ActiveOverlappingCharacter = Character; }

	FORCEINLINE void RemoveActiveOverlappingQuest() { ActiveOverlappingQuest = nullptr; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	class UAnimMontage* Armed_LightAttack_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* Armed_HeavyAttack_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* Armed_ComboLightAttack_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* Armed_ComboHeavyAttack_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* Unarmed_LightAttack_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* Unarmed_HeavyAttack_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* Armed_StandingReact_ForHeavy_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* Armed_Death_Montage;

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	void UpdateCombatTarget();

	UFUNCTION(BlueprintCallable)
	void ChangeCameraAngle();

	UFUNCTION(BlueprintCallable)
	void RemoveQuestLogFromScreen();

	UFUNCTION(BlueprintCallable)
	void ActivateLeftHandCollision();

	UFUNCTION(BlueprintCallable)
	void ActivateRightHandCollision();

	UFUNCTION(BlueprintCallable)
	void ActivateRightLegCollision();

	UFUNCTION(BlueprintCallable)
	void ActivateHeadCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCombatCollisions();

	/*UFUNCTION(BlueprintCallable)
	void DeactivateRightHandCollision();*/

	UFUNCTION(BlueprintCallable)
	bool CheckSaveGameAvailability();

	UFUNCTION()
	virtual void LeftHandCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void LeftHandCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void RightHandCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void RightHandCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void RightLegCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void RightLegCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void HeadCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void HeadCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
