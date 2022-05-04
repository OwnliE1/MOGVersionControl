// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MOGWeapon.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;
class AMOGChar;
class UParticleSystemComponent;
class AMOGEnemy;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Pickup UMETA(DisplayName = "Pickup"),
	EWS_Attack UMETA(DisplayName = "Attack")
};

USTRUCT(BlueprintType)
struct FWeaponAttributes 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FString WeaponName = FString(TEXT(""));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float WeaponDamage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float BlockAmount = 0.0f;
};

UCLASS()
class MOG_API AMOGWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMOGWeapon();

	AMOGChar* MyChar;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FWeaponAttributes WeaponAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponState")
	EWeaponState WeaponState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* PickupCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* CombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UParticleSystemComponent* MagicFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	USoundBase* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	USoundBase* HitSound_Light;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	USoundBase* HitSound_Hard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* WeaponLightAttack_Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	float LightAttackStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	float LightAttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* WeaponHardAttack_Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	float HardAttackStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	float HardAttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	bool bKeepFXAfterPickup;

	int32 HitNumber;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void CombatOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void Equip(AMOGChar* PlayerChar);

	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
	FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }

	void ChooseAndAttach(AMOGChar* Player);

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	void AITakeDamage(AMOGEnemy* MyEnemy);
};
