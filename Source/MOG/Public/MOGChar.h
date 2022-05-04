// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MOGChar.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	EMS_Walking		UMETA(DisplayName = "Walking"),
	EMS_Running		UMETA(DisplayName = "Running"),
	EMS_Sprinting	UMETA(DisplayName = "Sprinting")
};

class USpringArmComponent;
class UCameraComponent;
class AMOGWeapon;
class USphereComponent;
class UParticleSystemComponent;
class UParticleSystem;
class UAnimMontage;
class AMOGEnemy;
class AMOGPlayerController;

UCLASS()
class MOG_API AMOGChar : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMOGChar();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Controller")
	AMOGPlayerController* MyPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UParticleSystemComponent* BuffFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* HitFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	AMOGWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	AMOGWeapon* OverlappingWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy")
	AMOGEnemy* CombatTarget;

	bool bIsShiftKeyPressed;

	bool bIsCtrlKeyPressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement System")
	EMovementState MovementState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement System")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement System")
	float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement System")
	float WalkingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float HealthDrain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float HealthRecovery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float CurrentStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaDrain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRecovery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float MaxZoomIn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float MaxZoomOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float DefaultLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currency")
	int32 Souls;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float RecoveryDelay;

	UPROPERTY(BlueprintReadWrite, Category = "Timer")
	FTimerHandle StaminaRecoveryTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* Roll_Monatage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	float StamninaToRoll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	float RollSpeed;

	UAnimMontage* LightAttack_Montage;

	float StaminaToLightAttack;

	float LightAnimSpeed;

	UAnimMontage* HardAttack_Montage;
	
	float StaminaToHardAttack;

	float HardAnimSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* Die_Montage;

	UPROPERTY()
	UAnimInstance* AnimInstance;

	bool bIsMovingForward;

	bool bIsMovingRight;

	bool bIsMoving;

	bool bCanRecover;

	UPROPERTY(BlueprintReadWrite, Category = "Pickup")
	bool bCanPickup;

	UPROPERTY(BlueprintReadWrite, Category = "Animations")
	bool bIsInAction;

	UPROPERTY(BlueprintReadWrite, Category = "Animations")
	bool bRoll;

	UPROPERTY(BlueprintReadWrite, Category = "Animations")
	bool bAttack;

	UPROPERTY(BlueprintReadWrite, Category = "Animations")
	bool bIsDead;

	UPROPERTY(BlueprintReadWrite, Category = "Animations")
	bool bIsLightAttack;

	UPROPERTY(BlueprintReadWrite, Category = "Animations")
	bool bIsHardAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CombatTarget")
	bool bHasCombatTarget;

	UPROPERTY(BlueprintReadWrite, Category = "CombatTarget")
	FVector CombatTargetLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ShiftKeyUp();

	void ShiftKeyDown();

	void CtrlKeyUp();

	void CtrlKeyDown();
	
	void SetRecoveryBool();

	UFUNCTION(BlueprintCallable)
	void FKeyDown();

	void FKeyUp();

	void CameraZoomIn();

	void CamerZoomOut();

	void CameraZoomDefault();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void SetMovementState(EMovementState State);

	void SetIsMoving(bool bMovingForward, bool bMovingRight);

	void Die();

	void HealthSystem(float RecoveryRate);

	void StaminaSystem(float DrainRate, float RecoveryRate);

	UFUNCTION(BlueprintCallable)
	void SetWeapon(AMOGWeapon* Weapon);

	FORCEINLINE void SetOverlappingWeapon(AMOGWeapon* NewWeapon) { OverlappingWeapon = NewWeapon; }

	void Roll();

	void LightAttack();

	void HardAttack();

	UFUNCTION(BlueprintCallable)
	void ClearStaminaTimer();

	UFUNCTION(BlueprintCallable)
	void SetStaminaTimer(float TimeAmount);

	FORCEINLINE void SetCombatTarget(AMOGEnemy* Target) { CombatTarget = Target; }
	FORCEINLINE void SetHasCombatTarget(bool bHasTarget) { bHasCombatTarget = bHasTarget; }
};
