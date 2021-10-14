// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Engine/GameEngine.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Enemy.h"
#include "Kismet/KismetMathlibrary.h"
#include "MainPlayerController.h"
#include "NonPlayableCharacter.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Quest.h"
#include "QuestLog.h"
#include "InteractionInterface.h"
#include "QuestPickup.h"
#include "PaperSpriteComponent.h"
#include "VanaraSaveGame.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "VanaraStorageRoom.h"
#include "TimerManager.h"
#include "Consumables.h"
#include "Weapon_Base.h"
#include "Weapon_Gada.h"
#include "Weapon_Enemy.h"
#include "VanaraGameInstance.h"


// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LeftHandCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandCollisionBox"));
	LeftHandCollisionBox->SetupAttachment(GetMesh(), FName("hand_l"));
	//LeftHandCollisionBox->SetCollisionProfileName("NoCollision");

	RightHandCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandCollisionBox"));
	RightHandCollisionBox->SetupAttachment(GetMesh(), FName("hand_r"));
	//RightHandCollisionBox->SetCollisionProfileName("NoCollision");

	RightLegCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightLegCollisionBox"));
	RightLegCollisionBox->SetupAttachment(GetMesh(), FName("foot_r"));

	HeadCollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HeadCollisionCapsule"));
	HeadCollisionCapsule->SetupAttachment(GetMesh(), FName("head"));

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	GetCapsuleComponent()->SetCapsuleSize(46.5f, 110.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;	

	MinimapBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("MinimapBoom"));
	MinimapBoom->SetupAttachment(GetRootComponent());
	MinimapBoom->TargetArmLength = 600.f;
	MinimapBoom->SetRelativeRotation(FRotator(-90.f, 0.0f, 0.0f));

	MinimapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCapture"));
	MinimapCapture->SetupAttachment(MinimapBoom, USpringArmComponent::SocketName);

	IconSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("IconSprite"));
	IconSprite->SetupAttachment(GetRootComponent());

	QuestLog = CreateDefaultSubobject<UQuestLog>(TEXT("QuestLog"));

	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	//Stops Character from rotating to mouse angles
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//Makes character move in the intented direction than the camera facing direction.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f;

	MaxHealth = 100.f;
	Health = 100.f;

	MaxStamina = 500.f;
	Stamina = 200.f;

	MaxHonor = 100.f;
	Honor = 25.f;

	MaxFoodCount = 5;
	FoodCount = 0;
	MaxPotionCount = 3;
	PotionCount = 0;
	bIsActiveItemFood = true;

	HealthGainPerFoodIntake = 150;
	StaminaGainPerPotionIntake = 50; 

	RunningSpeed = 650.f;
	SprintingSpeed = 1200.f;
	FightingStateSpeed = 60.f;
	bShiftKeyDown = false;
	bInteractKeyDown = false;
	bLightAttackKeyDown = false;
	bHeavyAttackKeyDown = false;
	bIsHeavyAttack = false;

	bFightMode = false;

	bStartComboTimer = false;
	AttackDelay = 0.2f;
	ComboKeyCount = 2;
	LightAttackKeyTimesClicked = 0;
	HeavyAttackKeyTimesClicked = 0;

	bMovingFrontBack = false;
	bMovingRightLeft = false;

	bShowHonorMeter = false;
	HonorBarTimer = 0;

	CharacterStatus = ECharacterStatus::ECS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.f;
	MinStaminaStatus = 100.f;

	InterpSpeed = 10.f;
	bInterpToEnemy = false;

	bIsDropKeyDown = false;
	DropKeyDownTimer = 0.f;

	bHasCombatTarget = false;

	bContainsSaveData = false;

	bIsQuestDialogueWidgetCreated = false;

	CurrentAttackSelection = 0;

	Damage = 15.f;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GameInstance = Cast<UVanaraGameInstance>(GetGameInstance());

	BaseTurnRate = GameInstance->BaseTurnRate;
	BaseLookUpRate = GameInstance->BaseLookUpRate;

	MainPlayerController = Cast <AMainPlayerController>(GetController());

	LeftHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftHandCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftHandCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightHandCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightLegCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightLegCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightLegCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightLegCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	HeadCollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeadCollisionCapsule->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	HeadCollisionCapsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	HeadCollisionCapsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	LeftHandCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::LeftHandCollisionOnOverlapBegin);
	LeftHandCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::LeftHandCollisionOnOverlapEnd);
	RightHandCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::RightHandCollisionOnOverlapBegin);
	RightHandCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::RightHandCollisionOnOverlapEnd);
	RightLegCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::RightLegCollisionOnOverlapBegin);
	RightLegCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::RightLegCollisionOnOverlapEnd);
	HeadCollisionCapsule->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::HeadCollisionOnOverlapBegin);
	HeadCollisionCapsule->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::HeadCollisionOnOverlapEnd);

	//DeleteGame();

	//LoadGame();
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (CharacterStatus == ECharacterStatus::ECS_Dead) return;

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch(StaminaStatus)
	{
		case EStaminaStatus::ESS_Normal:
			if (bShiftKeyDown && (bMovingFrontBack || bMovingRightLeft))
			{
				if (Stamina - DeltaStamina <= MinStaminaStatus)
				{
					SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
					Stamina -= DeltaStamina;
				}
				else
				{
					Stamina -= DeltaStamina;
				}
				SetCharacterStatus(ECharacterStatus::ECS_Sprinting);
			}
			else
			{
				if (Stamina + DeltaStamina >= MaxStamina)
				{
					Stamina = MaxStamina;
				}
				else
				{
					Stamina += DeltaStamina;
				}
				if(CharacterStatus != ECharacterStatus::ECS_Fighting)
					SetCharacterStatus(ECharacterStatus::ECS_Normal);
			}
			break;
		case EStaminaStatus::ESS_BelowMinimum:
			if (bShiftKeyDown && (bMovingFrontBack || bMovingRightLeft))
			{
				if (Stamina - DeltaStamina <= 0.f)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
					Stamina = 0;
					SetCharacterStatus(ECharacterStatus::ECS_Normal);
				}
				else
				{
					Stamina -= DeltaStamina;
					SetCharacterStatus(ECharacterStatus::ECS_Sprinting);
				}
			}
			else
			{
				if (Stamina += DeltaStamina >= MinStaminaStatus)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Normal);
					Stamina += DeltaStamina;
				}
				else
				{
					Stamina += DeltaStamina;
				}
				SetCharacterStatus(ECharacterStatus::ECS_Normal);
			}
			break;
		case EStaminaStatus::ESS_Exhausted:
			if (bShiftKeyDown && (bMovingFrontBack || bMovingRightLeft))
			{
				Stamina = 0.f;
			}
			else
			{
				SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
				Stamina += DeltaStamina;
			}
			SetCharacterStatus(ECharacterStatus::ECS_Normal);
			break;
		case EStaminaStatus::ESS_ExhaustedRecovering:
			if (Stamina + DeltaStamina >= MinStaminaStatus)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetCharacterStatus(ECharacterStatus::ECS_Normal);
			break;
		default:
			break;
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();

		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}

	if (bShowHonorMeter && HonorBarTimer <= 3.f)
	{
		HonorBarTimer += DeltaTime;
	}	
	else
	{
		bShowHonorMeter = false;
		HonorBarTimer = 0.f;
	}

	if (CharacterStatus == ECharacterStatus::ECS_Fighting && FightingStateTimer <= 7.f)
	{
		FightingStateTimer += DeltaTime;
	}
	else
	{
		if(CharacterStatus != ECharacterStatus::ECS_Sprinting)
			SetCharacterStatus(ECharacterStatus::ECS_Normal);

		FightingStateTimer = 0.f;
	}

	if (bIsDropKeyDown)
	{
		DropKeyDownTimer += DeltaTime;
	}
		
}

FRotator AMainCharacter::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::ShiftKeyUp);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::InteractKeyDown);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMainCharacter::InteractKeyUp);

	PlayerInputComponent->BindAction("Display Quest Log", IE_Pressed, this, &AMainCharacter::QuestLogKeyDown);
	PlayerInputComponent->BindAction("Display Quest Log", IE_Released, this, &AMainCharacter::QuestLogKeyUp);

	PlayerInputComponent->BindAction("Accept", IE_Pressed, this, &AMainCharacter::AcceptKeyDown);
	PlayerInputComponent->BindAction("Accept", IE_Released, this, &AMainCharacter::AcceptKeyUp);

	PlayerInputComponent->BindAction("Honor Bar", IE_Pressed, this, &AMainCharacter::HonorMeterKeyDown);
	PlayerInputComponent->BindAction("Honor Bar", IE_Released, this, &AMainCharacter::HonorMeterKeyUp);

	PlayerInputComponent->BindAction("Light Attack", IE_Pressed, this, &AMainCharacter::LightAttackKeyDown);
	PlayerInputComponent->BindAction("Light Attack", IE_Released, this, &AMainCharacter::LightAttackKeyUp);

	PlayerInputComponent->BindAction("Heavy Attack", IE_Pressed, this, &AMainCharacter::HeavyAttackKeyDown);
	PlayerInputComponent->BindAction("Heavy Attack", IE_Released, this, &AMainCharacter::HeavyAttackKeyUp);

	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &AMainCharacter::DropKeyDown);
	PlayerInputComponent->BindAction("Drop", IE_Released, this, &AMainCharacter::DropKeyUp);

	PlayerInputComponent->BindAction("Switch Item", IE_Pressed, this, &AMainCharacter::TabKeyDown);
	PlayerInputComponent->BindAction("Switch Item", IE_Released, this, &AMainCharacter::TabKeyUp);

	PlayerInputComponent->BindAction("Use Item", IE_Pressed, this, &AMainCharacter::UseKeyDown);
	PlayerInputComponent->BindAction("Use Item", IE_Released, this, &AMainCharacter::UseKeyUp);

	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMainCharacter::EscapeKeyDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMainCharacter::EscapeKeyUp);

	PlayerInputComponent->BindAction("Adjust Camera Angle", IE_Pressed, this, &AMainCharacter::ChangeCameraAngle);

	PlayerInputComponent->BindAxis("Move Front/Back", this, &AMainCharacter::MoveFrontBack);
	PlayerInputComponent->BindAxis("Move RIght/Left", this, &AMainCharacter::MoveRightLeft);

	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::TurnLeftRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUpDown);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);
}

bool AMainCharacter::FreeToMove(float Value)
{
	if (MainPlayerController) {
		return (
			Value != 0.0f &&  
			(CharacterStatus != ECharacterStatus::ECS_Dead) && 
			!MainPlayerController->bPauseMenuVisible
		);
	}
	return false;
}

void AMainCharacter::MoveFrontBack(float Input)
{
	bMovingFrontBack = false;
	if (!bAttacking && FreeToMove(Input))
	{
		
		if (CharacterStatus != ECharacterStatus::ECS_Fighting)
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Input);
		}
		else
		{
			FVector Direction = this->GetActorForwardVector();
			AddMovementInput(Direction, Input);
		}

		bMovingFrontBack = true;
	}
}

void AMainCharacter::MoveRightLeft(float Input)
{
	bMovingRightLeft = false;
	if (!bAttacking && FreeToMove(Input))
	{
		
		if (CharacterStatus != ECharacterStatus::ECS_Fighting)
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			AddMovementInput(Direction, Input);
		}
		else
		{
			FVector Direction = this->GetActorRightVector();
			AddMovementInput(Direction, Input);
		}

		bMovingRightLeft = true;
	}
}

void AMainCharacter::TurnLeftRight(float Input)
{
	if (FreeToMove(Input))
	{
		
		AddControllerYawInput(Input * GameInstance->BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AMainCharacter::LookUpDown(float Input)
{
	if (FreeToMove(Input))
	{
		
		AddControllerPitchInput(Input * GameInstance->BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void AMainCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * GameInstance->BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * GameInstance->BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::InteractKeyDown()
{
	bInteractKeyDown = true;

	if (CharacterStatus == ECharacterStatus::ECS_Dead) return;
	
	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible) return;
	}
	
	if (ActiveOverlappingItem)
	{
		AWeapon_Gada* Weapon = Cast<AWeapon_Gada>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
			if (MainPlayerController)
			{
				MainPlayerController->RemoveInteractMessage();
			}
		}
		AQuestPickup* QuestPickup = Cast<AQuestPickup>(ActiveOverlappingItem);
		if (QuestPickup)
		{
			IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(QuestPickup);
			if (InteractionInterface)
			{
				InteractionInterface->Execute_Interact(QuestPickup, this);
			}
		}
		AConsumables* PickUpItem = Cast<AConsumables>(ActiveOverlappingItem);
		if (PickUpItem)
		{
			PickUpItem->OnPickupBP(this);
			SetActiveOverlappingItem(nullptr);
			if (MainPlayerController)
			{
				MainPlayerController->RemoveInteractMessage();
			}
		}
	}

	if (ActiveOverlappingCharacter)
	{
		ANonPlayableCharacter* NonPlayableCharacter = Cast<ANonPlayableCharacter>(ActiveOverlappingCharacter);
		if (NonPlayableCharacter)
		{
			TArray<AActor*> AttachedActors;
			NonPlayableCharacter->GetAttachedActors(AttachedActors);
			if (AttachedActors.Num() > 0)
			{
				for (auto Actor : AttachedActors)
				{
					ActiveOverlappingQuest = Cast<AQuest>(AttachedActors[0]);
					if (ActiveOverlappingQuest)
					{
						if (ActiveOverlappingQuest->PreRequisite)
						{
							if (!ActiveOverlappingQuest->PreRequisite->bIsQuestCompleted)
							{
								MainPlayerController->DisplayGeneralDialogue();
								break;
							}
						}
						if (!ActiveOverlappingQuest->bIsQuestedAccpeted)
						{
							if (MainPlayerController && !bIsQuestDialogueWidgetCreated)
							{
								MainPlayerController->CreateQuestDialogueWidget();
								bIsQuestDialogueWidgetCreated = true;
								MainPlayerController->RemoveInteractMessage();
							}
						}
						else
						{
							if (MainPlayerController)
							{
								MainPlayerController->RemoveInteractMessage();
								if (!(ActiveOverlappingQuest->bIsQuestCompleted))
								{
									MainPlayerController->DisplayHelpMessage();
								}
								else
									MainPlayerController->DisplayThankingMessage();
							}
						}
					}
				}
			}
			IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(NonPlayableCharacter);
			if (InteractionInterface)
			{
				MainPlayerController->RemoveInteractMessage();
				InteractionInterface->Execute_Interact(NonPlayableCharacter,this);
			}
		}
	}
}

void AMainCharacter::InteractKeyUp()
{
	bInteractKeyDown = false;
}

void AMainCharacter::QuestLogKeyDown()
{
	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible) return;

		MainPlayerController->RemoveHUDOverlay();
		MainPlayerController->CreateQuestLogWidget();
		MainPlayerController->SetInputMode(FInputModeUIOnly());
		MainPlayerController->bShowMouseCursor = true;
	}
}

void AMainCharacter::RemoveQuestLogFromScreen()
{
	if (MainPlayerController)
	{
		MainPlayerController->RemoveQuestLog();
		MainPlayerController->SetInputMode(FInputModeGameOnly());
		MainPlayerController->bShowMouseCursor = false;
		MainPlayerController->DisplayHUDOverlay();
	}
}

void AMainCharacter::QuestLogKeyUp()
{
	
}

void AMainCharacter::AcceptKeyDown()
{
	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible) return;
	}

	if (ActiveOverlappingCharacter)
	{
		if (ActiveOverlappingQuest)
		{
			if (QuestLog)
			{
				QuestLog->AddQuestToLog(ActiveOverlappingQuest);
				ActiveOverlappingQuest->bIsQuestedAccpeted = true;
				ActiveOverlappingQuest->bIsQuestActive = true;

				if (MainPlayerController)
				{
					MainPlayerController->RemoveInteractMessage();
					MainPlayerController->RemoveQuestDialogue();
					RemoveActiveOverlappingQuest();
					bIsQuestDialogueWidgetCreated = false;
				}
			}
		}
	}
}

void AMainCharacter::AcceptKeyUp()
{

}

void AMainCharacter::HonorMeterKeyDown()
{
	bShowHonorMeter = true;
}

void AMainCharacter::HonorMeterKeyUp()
{
	
}

void AMainCharacter::DecreamentHealth(float Amount)
{
	if ((Health - Amount) <= 0.f)
	{
		Health -= Amount;
		Die();
	}
	else
	{
		Health -= Amount;
	}
}

void AMainCharacter::LightAttackKeyDown()
{
	if (CharacterStatus == ECharacterStatus::ECS_Dead) return;

	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible) return;
	}

	if (!bAttacking)
	{
		bIsHeavyAttack = false;
		++LightAttackKeyTimesClicked;

		if (!bStartComboTimer)
		{
			bStartComboTimer = true;
			GetWorldTimerManager().SetTimer(ComboTimer, this, &AMainCharacter::AttackType, AttackDelay);
		}
	}
}

void AMainCharacter::LightAttackKeyUp()
{
	
}

void AMainCharacter::HeavyAttackKeyDown()
{
	if (CharacterStatus == ECharacterStatus::ECS_Dead) return;

	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible) return;
	}

	if (!bAttacking)
	{
		bIsHeavyAttack = true;
		++HeavyAttackKeyTimesClicked;

		if (!bStartComboTimer)
		{
			bStartComboTimer = true;
			GetWorldTimerManager().SetTimer(ComboTimer, this, &AMainCharacter::AttackType, AttackDelay);
		}
	}
}

void AMainCharacter::HeavyAttackKeyUp()
{

}

void AMainCharacter::DropKeyDown()
{
	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible) return;
	}
	
	bIsDropKeyDown = true;
}

void AMainCharacter::DropKeyUp()
{
	bIsDropKeyDown = false;
	if (DropKeyDownTimer > 0.5f)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Detach(this);
		}
	}
	DropKeyDownTimer = 0.f;
}

void AMainCharacter::TabKeyDown()
{
	ToggleActiveConsumable();
}

void AMainCharacter::TabKeyUp()
{

}

void AMainCharacter::UseKeyDown()
{
	if (bIsActiveItemFood)
	{
		if (FoodCount > 0)
		{
			IncreamentHealth();
		}
	}
	else
	{
		if (PotionCount > 0)
		{
			IncreamentStamina();
		}
	}
}

void AMainCharacter::UseKeyUp()
{

}

void AMainCharacter::ToggleActiveConsumable()
{
	bIsActiveItemFood = bIsActiveItemFood ? false : true;
}

void AMainCharacter::SetCharacterStatus(ECharacterStatus Status)
{
	CharacterStatus = Status;
	if (CharacterStatus == ECharacterStatus::ECS_Sprinting)
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else if(CharacterStatus == ECharacterStatus::ECS_Fighting)
	{
		GetCharacterMovement()->MaxWalkSpeed = FightingStateSpeed;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMainCharacter::ShiftKeyDown()
{
	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible) return;
	}

	bShiftKeyDown = true;
}

void AMainCharacter::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void AMainCharacter::AttackType()
{
	if (!bIsHeavyAttack)
	{
		if (LightAttackKeyTimesClicked >= ComboKeyCount)
		{
			if (EquippedWeapon)
			{
				EquippedWeapon->Damage = 25.f;
				Attack(Armed_ComboLightAttack_Montage, 1.2f, 1.8f, 1.2f);
			}
			else
			{
				Damage = 15.f;
				Attack(Unarmed_LightAttack_Montage, 1.5f, 1.8f, 1.2f);
			}
		}
		else
		{
			if (EquippedWeapon)
			{
				EquippedWeapon->Damage = 25.f;
				Attack(Armed_LightAttack_Montage, 1.2f, 1.8f, 1.2f);
			}
			else
			{
				Damage = 15.f;
				Attack(Unarmed_LightAttack_Montage, 1.5f, 1.8f, 1.2f);
			}
		}
	}
	else 
	{
		if (HeavyAttackKeyTimesClicked >= ComboKeyCount)
		{
			if (EquippedWeapon)
			{
				EquippedWeapon->Damage = 40.f;
				Attack(Armed_ComboHeavyAttack_Montage, 1.0f, 1.5f, 1.2f);
			}
			else
			{
				Damage = 30.f;
				Attack(Unarmed_HeavyAttack_Montage, 1.0f, 1.2f, 1.2f);
			}
		}
		else
		{
			if (EquippedWeapon)
			{
				EquippedWeapon->Damage = 40.f;
				Attack(Armed_HeavyAttack_Montage, 1.0f, 1.5f, 1.2f);
			}
			else
			{
				Damage = 30.f;
				Attack(Unarmed_HeavyAttack_Montage, 1.0f, 1.2f, 1.2f);
			}
		}
	}
}

void AMainCharacter::Attack(UAnimMontage* CurrentMontage, float Attack1_Speed, float Attack2_Speed, float Attack3_Speed)
{
	if (!bAttacking && (CharacterStatus != ECharacterStatus::ECS_Dead))
	{
		if (!EquippedWeapon && bFightMode)
		{
			SetCharacterStatus(ECharacterStatus::ECS_Fighting);
			FightingStateTimer = 0.f;
		}

		LightAttackKeyTimesClicked = 0;
		HeavyAttackKeyTimesClicked = 0;
		bStartComboTimer = false;
		bAttacking = true;
		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CurrentMontage)
		{
			int32 AttackSelection = FMath::RandRange(0, 2);
			CurrentAttackSelection = AttackSelection + 1;
			switch(AttackSelection)
			{
				case 0:
					AnimInstance->Montage_Play(CurrentMontage, Attack1_Speed);
					AnimInstance->Montage_JumpToSection(FName("Attack 1"), CurrentMontage);
					break;
				case 1:
					AnimInstance->Montage_Play(CurrentMontage, Attack2_Speed);
					AnimInstance->Montage_JumpToSection(FName("Attack 2"), CurrentMontage);
					break;
				case 2:
					AnimInstance->Montage_Play(CurrentMontage, Attack3_Speed);
					AnimInstance->Montage_JumpToSection(FName("Attack 3"), CurrentMontage);
					break;
				default:
					;
			}
		}
		if (EquippedWeapon->SwingSound)
		{
			//UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
		}
	}
}

void AMainCharacter::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0)
	{
		if (MainPlayerController)
		{
			MainPlayerController->RemoveEnemyHealthBar();
		}
		return;
	}

	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);
	if (ClosestEnemy)
	{
		float MinDistance = (ClosestEnemy->GetActorLocation() - GetActorLocation()).Size();

		for (auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - GetActorLocation()).Size();
				if (DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}
		if (MainPlayerController)
		{
			MainPlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
}

void AMainCharacter::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);
}

void AMainCharacter::ActivateLeftHandCollision()
{
	LeftHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMainCharacter::ActivateRightHandCollision()
{
	RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMainCharacter::ActivateRightLegCollision()
{
	RightLegCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMainCharacter::ActivateHeadCollision()
{
	HeadCollisionCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMainCharacter::DeactivateCombatCollisions()
{
	LeftHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightLegCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeadCollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CurrentAttackSelection = 0;
}

bool AMainCharacter::IncreamentFood()
{
	if (FoodCount < MaxFoodCount)
	{
		FoodCount++;
		return true;
	}
	return false;
}

bool AMainCharacter::IncreamentPotion()
{
	if (PotionCount < MaxPotionCount)
	{
		PotionCount++;
		return true;
	}
	return false;
}

void AMainCharacter::IncreamentHealth()
{
	if (CharacterStatus != ECharacterStatus::ECS_Dead && Health < MaxHealth)
	{
		Health = ((Health += HealthGainPerFoodIntake) >= MaxHealth) ? MaxHealth : (Health + HealthGainPerFoodIntake);
		FoodCount--;
	}
}

void AMainCharacter::IncreamentStamina()
{
	if (CharacterStatus != ECharacterStatus::ECS_Dead && Stamina < MaxStamina)
	{
		Stamina = ((Stamina += StaminaGainPerPotionIntake) >= MaxStamina) ? MaxStamina : (Stamina + StaminaGainPerPotionIntake);
		PotionCount--;
	}
}

void AMainCharacter::Jump()
{
	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible) return;
	}

	if (CharacterStatus != ECharacterStatus::ECS_Dead && !GetAttackingStatus())
	{
		ACharacter::Jump();
	}
}

void AMainCharacter::EscapeKeyDown()
{
	bEscapeKeyDown = true;

	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}

void AMainCharacter::EscapeKeyUp()
{
	bEscapeKeyDown = false;
}

void AMainCharacter::Die()
{
	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible) return;
	}

	if (CharacterStatus == ECharacterStatus::ECS_Dead)
		return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Armed_Death_Montage)
	{
		int32 DeathSelection = FMath::RandRange(0, 2);
		switch (DeathSelection)
		{
		case 0:
			AnimInstance->Montage_Play(Armed_Death_Montage, 1.5f);
			AnimInstance->Montage_JumpToSection(FName("Death 1"), Armed_Death_Montage);
			break;
		case 1:
			AnimInstance->Montage_Play(Armed_Death_Montage, 1.5f);
			AnimInstance->Montage_JumpToSection(FName("Death 2"), Armed_Death_Montage);
			break;
		case 2:
			AnimInstance->Montage_Play(Armed_Death_Montage, 1.5f);
			AnimInstance->Montage_JumpToSection(FName("Death 3"), Armed_Death_Montage);
			break;
		default:
			;
		}
	}
	SetCharacterStatus(ECharacterStatus::ECS_Dead);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMainCharacter::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	MainPlayerController->RemoveHUDOverlay();
	MainPlayerController->DisplayDeathMenu();
	MainPlayerController->SetInputMode(FInputModeUIOnly());
	MainPlayerController->bShowMouseCursor = true;
	
	//GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

void AMainCharacter::ChangeCameraAngle()
{
	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible) return;
	}

	if(CameraBoom->TargetArmLength < 500.f)
		CameraBoom->TargetArmLength = 600.f;
	else
		CameraBoom->TargetArmLength = 400.f;
}

void AMainCharacter::ShowPickupLocations()
{
	for (int32 i = 0; i < PickupLocations.Num(); i++)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, PickupLocations[i] , 10.f, 8, FLinearColor::Yellow, 10.f, .5f);
	}
}

void AMainCharacter::SetEquippedWeapon(AWeapon_Gada* WeaponToSet)
{ 
	/*if (EquippedWeapon)
	{
		EquippedWeapon->Detach(this);
	}*/
	EquippedWeapon = WeaponToSet; 
}

void AMainCharacter::PlaySwingSound()
{
	if (EquippedWeapon)
	{
		if (EquippedWeapon->SwingSound)
		{
			UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
		}
	}
}

void AMainCharacter::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

float AMainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
	AWeapon_Enemy* Weapon = Cast<AWeapon_Enemy>(DamageCauser);
	if(Enemy || Weapon)
	{
		if (Weapon)
		{
			AActor* Weilder = Weapon->GetAttachParentActor();
			Enemy = Cast<AEnemy>(Weilder);
		}
		FRotator DeltaRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Enemy->GetActorLocation());
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
			if (bAttacking)
			{
				bAttacking = false;
				AttackEnd();
				if (GetEquippedWeapon())
				{
					EquippedWeapon->DeactivateCollision();
				}
				{
					DeactivateCombatCollisions();
				}
			}

			switch (HitFromDirection)
			{
				case 0:
				case 7:
					AnimInstance->Montage_Play(Armed_StandingReact_ForHeavy_Montage, 0.9f);
					AnimInstance->Montage_JumpToSection(FName("HitFromFront"), Armed_StandingReact_ForHeavy_Montage);
					break;
				case 1:
				case 2:
					AnimInstance->Montage_Play(Armed_StandingReact_ForHeavy_Montage, 0.9f);
					AnimInstance->Montage_JumpToSection(FName("HitFromRight"), Armed_StandingReact_ForHeavy_Montage);
					break;
				case 3:
				case 4:
					AnimInstance->Montage_Play(Armed_StandingReact_ForHeavy_Montage, 0.9f);
					AnimInstance->Montage_JumpToSection(FName("HitFromBack"), Armed_StandingReact_ForHeavy_Montage);
					break;
				case 5:
				case 6:
					AnimInstance->Montage_Play(Armed_StandingReact_ForHeavy_Montage, 0.9f);
					AnimInstance->Montage_JumpToSection(FName("HitFromLeft"), Armed_StandingReact_ForHeavy_Montage);
					break;
				default:
					break;
			}
		}
	}

	DecreamentHealth(DamageAmount);

	return DamageAmount;
}

bool AMainCharacter::CheckSaveGameAvailability()
{
	UVanaraSaveGame* LoadGameInstance = Cast<UVanaraSaveGame>(UGameplayStatics::CreateSaveGameObject(UVanaraSaveGame::StaticClass()));

	LoadGameInstance = Cast<UVanaraSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	if (LoadGameInstance)
	{
		return true;
	}

	return false;
}

void AMainCharacter::LeftHandCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Enemy, Damage, MainPlayerController, this, DamageTypeClass);
			}
		}
	}
}


void AMainCharacter::LeftHandCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


void AMainCharacter::RightHandCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Enemy, Damage, MainPlayerController, this, DamageTypeClass);
			}
		}
	}
}


void AMainCharacter::RightHandCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AMainCharacter::RightLegCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Enemy, Damage, MainPlayerController, this, DamageTypeClass);
			}
		}
	}
}


void AMainCharacter::RightLegCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AMainCharacter::HeadCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Enemy, Damage, MainPlayerController, this, DamageTypeClass);
			}
		}
	}
}


void AMainCharacter::HeadCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
