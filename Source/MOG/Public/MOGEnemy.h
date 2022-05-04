// Fill out your copyright notice in the Description page of Project Se{ttings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MOGEnemy.generated.h"

class AAIController;
class AMOGChar;
class USphereComponent;
class UAnimMontage;
class UParticleSystem;
class AMOGEnemyWeapon;
class UAnimInstance;
class UBoxComponent;
class USkeletalMeshComponent;
class USoundBase;


UENUM(BlueprintType)
enum class EEnemyStatus : uint8
{
	EES_Patroling	UMETA(DisplayName = "Patroling"),
	EES_Chasing		UMETA(DisplayName = "Chasing"),
	EES_Combat		UMETA(DisplayName = "Combat"),
	EES_Dead		UMETA(DisplayName = "Dead")
};

USTRUCT(BlueprintType)
struct FEnemyAttributes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FString EnemyName = FString(TEXT(""));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float EnemyMaxHealth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float EnemyCurrentHealth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float EnemyHealthRecovery = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Armor = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Damage;
};

UCLASS()
class MOG_API AMOGEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMOGEnemy();

	UAnimInstance* AnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UBoxComponent* CombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* EnemyWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FEnemyAttributes EnemyAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyFX")
	UParticleSystem* HitFX;

	AAIController* AIController;

	UAnimInstance* EnemyAnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	UAnimMontage* Attack_Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyStatus")
	EEnemyStatus EnemyStatus;

	/*
	UPROPERTY(BlueprintReadWrite, Category = "PlayerRef")
	AMOGChar* MyChar;
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CombatTarget")
	AMOGChar* CombatTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* AggroSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* CombatSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* ChasingSphere;

	FTimerHandle AttackDelayTimer;

	FVector EnemySpawnLocation;

	FVector ActualLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bIsOverlappingCombatSphere;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bIsAttacking;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bIsDead;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bDied;

	float InterpSpeed;

	bool bInterpToPlayer;

	bool bIsHardAttack;

	UPROPERTY(BlueprintReadWrite, Category = "Timer")
	bool bIsTimerActive;

	int32 HitNumber;

	FRotator GetLookAtRotationYaw(FVector Target);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(AMOGChar* Target);

	void GoBackToStartPosition(FVector StartPosition) const;

	UFUNCTION()
	void AggroOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void AggroOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetEnemyStatus(EEnemyStatus Status);

	UFUNCTION()
	void CombatOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ChasingOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void CombatOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void ChasingOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void EnemyHealthSystem(float RecoveryRate);

	void EnemyDeath();

	UFUNCTION(BlueprintCallable)
	void AttackSystem();

	UFUNCTION(BlueprintCallable)
	void SetInterpToPlayer(bool Interp);

	UFUNCTION()
	void WeaponOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void WeaponOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void DamageToPlayer(AMOGChar* Player);

	UFUNCTION(BlueprintCallable)
	void ActivateCollsion();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();
};
