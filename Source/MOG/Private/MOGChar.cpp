// Fill out your copyright notice in the Description page of Project Settings.


#include "MOGChar.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MOGWeapon.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MOGPlayerController.h"
#include "MOGEnemy.h"


// Sets default values
AMOGChar::AMOGChar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup Player Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetupAttachment(RootComponent);

	BuffFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BuffFX"));
	BuffFX->SetupAttachment(RootComponent);

	// Setup Movement System
	bIsShiftKeyPressed = false;
	bCanRecover = true;
	bIsMoving = false;

	// Setup Gameplay Mechanics
	bCanPickup = false;
	bRoll = true;
	bAttack = true;
	bIsLightAttack = false;
	bIsHardAttack = false;
	bIsInAction = false;
	bIsDead = false;
	bHasCombatTarget = false;

	StamninaToRoll = 10.0f;
	StaminaToLightAttack = 0.0f;
	StaminaToHardAttack = 0.0f;

	LightAnimSpeed = 1.0f;
	HardAnimSpeed = 1.0f;
	RollSpeed = 1.0f;

	WalkingSpeed = 250.0f;
	RunningSpeed = 350.0f;
	SprintSpeed = 450.0f;

	// Setup Health System
	MaxHealth = 100.0f;
	CurrentHealth = 100.0f;
	HealthDrain = 10.0f;
	HealthRecovery = 5.0f;

	// Setup Stamina System
	MaxStamina = 100.0f;
	CurrentStamina = 100.0f;
	StaminaDrain = 20.0f;
	StaminaRecovery = 10.0f;

	Souls = 999;
}

// Called when the game starts or when spawned
void AMOGChar::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	CameraBoom->TargetArmLength = DefaultLength;

	AnimInstance = GetMesh()->GetAnimInstance();

	MyPlayerController = Cast<AMOGPlayerController>(GetController());
}

void AMOGChar::ShiftKeyUp()
{
	SetMovementState(EMovementState::EMS_Running);
	bIsShiftKeyPressed = false;

	SetStaminaTimer(RecoveryDelay);
}

void AMOGChar::ShiftKeyDown()
{
	SetMovementState(EMovementState::EMS_Sprinting);
	bIsShiftKeyPressed = true;

	ClearStaminaTimer();

	if (bIsMoving)
	{
		bCanRecover = false;
	}
}

void AMOGChar::CtrlKeyUp()
{
	SetMovementState(EMovementState::EMS_Running);
	bIsCtrlKeyPressed = false;
}

void AMOGChar::CtrlKeyDown()
{
	SetMovementState(EMovementState::EMS_Walking);
	bIsCtrlKeyPressed = true;
}

void AMOGChar::SetRecoveryBool()
{
	bCanRecover = true;
}

void AMOGChar::FKeyDown()
{
	bCanPickup = true;

	if (OverlappingWeapon)
	{
		AMOGWeapon* MyWeapon = Cast<AMOGWeapon>(OverlappingWeapon);
		if (MyWeapon)
		{
			StaminaToLightAttack = MyWeapon->LightAttackStamina;
			StaminaToHardAttack = MyWeapon->HardAttackStamina;

			LightAttack_Montage = MyWeapon->WeaponLightAttack_Montage;
			HardAttack_Montage = MyWeapon->WeaponHardAttack_Montage;

			LightAnimSpeed = MyWeapon->LightAttackSpeed;
			HardAnimSpeed = MyWeapon->HardAttackSpeed;

			MyWeapon->Equip(this);
		}
	}
}

void AMOGChar::FKeyUp()
{
	bCanPickup = false;
}

void AMOGChar::CameraZoomIn()
{
	if (CameraBoom->TargetArmLength >= MaxZoomIn)
	{
		CameraBoom->TargetArmLength -= 10.0f;
	}
}

void AMOGChar::CamerZoomOut()
{
	if (CameraBoom->TargetArmLength <= MaxZoomOut)
	{
		CameraBoom->TargetArmLength += 10.0f;
	}
}

void AMOGChar::CameraZoomDefault()
{
	CameraBoom->TargetArmLength = DefaultLength;
}

// Called every frame
void AMOGChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetIsMoving(bIsMovingForward, bIsMovingRight);

	float DeltaStaminaRecovery = StaminaRecovery * DeltaTime;
	float DeltaStaminaDrain = StaminaDrain * DeltaTime;

	float DeltaHealthRecovery = HealthRecovery * DeltaTime;

	if (!bIsDead)
	{
		HealthSystem(DeltaHealthRecovery);
		StaminaSystem(DeltaStaminaDrain, DeltaStaminaRecovery);
	}
	else
	{
		if (CurrentStamina > 0)
		{
			CurrentStamina -= DeltaStaminaDrain * 3.0f;
		}
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MyPlayerController)
		{
			MyPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}

// Called to bind functionality to input
void AMOGChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMOGChar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMOGChar::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AMOGChar::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &AMOGChar::AddControllerYawInput);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMOGChar::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMOGChar::ShiftKeyUp);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AMOGChar::CtrlKeyDown);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AMOGChar::CtrlKeyUp);

	PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &AMOGChar::FKeyDown);
	PlayerInputComponent->BindAction("Pickup", IE_Released, this, &AMOGChar::FKeyUp);

	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &AMOGChar::Roll);

	PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &AMOGChar::LightAttack);

	PlayerInputComponent->BindAction("HardAttack", IE_Pressed, this, &AMOGChar::HardAttack);

	PlayerInputComponent->BindAction("CameraZoomIn", IE_Pressed, this, &AMOGChar::CameraZoomIn);
	PlayerInputComponent->BindAction("CameraZoomOut", IE_Pressed, this, &AMOGChar::CamerZoomOut);
	PlayerInputComponent->BindAction("CameraZoomDefault", IE_Pressed, this, &AMOGChar::CameraZoomDefault);
}

void AMOGChar::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		if (bIsInAction)
		{
			bIsMovingForward = false;
		}
		else
		{
			bIsMovingForward = true;
		}

		if (bIsCtrlKeyPressed)
		{
			SetMovementState(EMovementState::EMS_Walking);
		}
	}
	else
	{
		bIsMovingForward = false;
	}
}

void AMOGChar::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

		if (bIsInAction)
		{
			bIsMovingRight = false;
		} 
		else
		{
			bIsMovingRight = true;
		}

		if (bIsCtrlKeyPressed)
		{
			SetMovementState(EMovementState::EMS_Walking);
		}
	}
	else
	{
		bIsMovingRight = false;
	}
}

void AMOGChar::SetMovementState(EMovementState State)
{
	MovementState = State;

	switch (MovementState)
	{
	case EMovementState::EMS_Walking:

		GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;

		break;
	case EMovementState::EMS_Running:

		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;

		break;
	case EMovementState::EMS_Sprinting:

		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

		break;
	default:
		break;
	}
}

void AMOGChar::SetIsMoving(bool bMovingForward, bool bMovingRight)
{
	if (bMovingForward || bMovingRight)
	{
		bIsMoving = true;
	}
	else
	{
		bIsMoving = false;
	}
}

void AMOGChar::Die()
{
	bIsDead = true;

	DisableInput(GetWorld()->GetFirstPlayerController());
}

void AMOGChar::HealthSystem(float RecoveryRate)
{
	if (CurrentHealth <= 0.0f)
	{
		Die();
	}

	// Recover Health over Time
	if (CurrentHealth > MaxHealth)
	{
		CurrentHealth = MaxHealth;
	}
	else
	{
		CurrentHealth += RecoveryRate;
	}
}

void AMOGChar::StaminaSystem(float DrainRate, float RecoveryRate)
{
	if (bIsShiftKeyPressed && bIsMoving)
	{
		if (CurrentStamina > 0.0f)
		{
			SetMovementState(EMovementState::EMS_Sprinting);
			CurrentStamina -= DrainRate;
		}
		else
		{
			SetMovementState(EMovementState::EMS_Running);
		}
	}
	else
	{
		if (CurrentStamina > MaxStamina)
		{
			CurrentStamina = MaxStamina;
		}
		else
		{
			SetMovementState(EMovementState::EMS_Running);

			if (bCanRecover)
			{
				CurrentStamina += RecoveryRate;
			}
		}
	}
}

void AMOGChar::SetWeapon(AMOGWeapon* Weapon)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}

	EquippedWeapon = Weapon;
}

void AMOGChar::Roll()
{
	if (bRoll)
	{
		//GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Red, TEXT("Rollen"), false, FVector2D(2.0f, 2.0f));
		if (CurrentStamina >= StamninaToRoll)
		{
			CurrentStamina -= StamninaToRoll;

			bCanRecover = false;

			if (AnimInstance)
			{
				AnimInstance->Montage_Play(Roll_Monatage, RollSpeed);
			}
		}
	}
}

void AMOGChar::LightAttack()
{
	if (EquippedWeapon)
	{
		if (bAttack)
		{
			if (CurrentStamina >= StaminaToLightAttack)
			{
				CurrentStamina -= StaminaToLightAttack;

				bCanRecover = false;

				bIsLightAttack = true;

				if (AnimInstance)
				{
					AnimInstance->Montage_Play(LightAttack_Montage, LightAnimSpeed);
				}
			}
		}
	}
}

void AMOGChar::HardAttack()
{
	if (EquippedWeapon)
	{
		if (bAttack)
		{
			if (CurrentStamina >= StaminaToHardAttack)
			{
				CurrentStamina -= StaminaToHardAttack;

				bCanRecover = false;

				bIsHardAttack = true;

				if (AnimInstance)
				{
					AnimInstance->Montage_Play(HardAttack_Montage, HardAnimSpeed);
				}
			}
		}
	}
}

void AMOGChar::ClearStaminaTimer()
{
	GetWorldTimerManager().ClearTimer(StaminaRecoveryTimer);
}

void AMOGChar::SetStaminaTimer(float TimeAmount)
{
	GetWorldTimerManager().SetTimer(StaminaRecoveryTimer, this, &AMOGChar::SetRecoveryBool, TimeAmount, false);
}
