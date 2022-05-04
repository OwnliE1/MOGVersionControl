// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MOGItem.generated.h"

class UStaticMeshComponent;
class UParticleSystemComponent;
class USphereComponent;
class USoundBase;
class AMOGChar;

USTRUCT(BlueprintType)
struct FItemAttributes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FString ItemName = FString(TEXT(""));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FString ItemCategory = FString(TEXT(""));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int32 Uses = 0;
};

UCLASS()
class MOG_API AMOGItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMOGItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FItemAttributes ItemAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* CollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UParticleSystemComponent* StartEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | FX")
	bool bActiveAtOverlap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | FX", meta = (EditCondition = "bActiveAtOverlap"))
	UParticleSystem* OverlapEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | FX", meta = (EditCondition = "bActiveAtOverlap"))
	bool bAttachToPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | Sound")
	USoundBase* ItemSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | Sound")
	USoundBase* PlayerHitSound;

	// Damage System
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | Damage", meta = (EditCondition = "!bActivateHealingSystem"))
	bool bActivateDamageSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | Damage", meta = (EditCondition = "bActivateDamageSystem"))
	float DamageToPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | Damage", meta = (EditCondition = "bActivateDamageSystem"))
	float DamageToAI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | Damage", meta = (EditCondition = "bActivateDamageSystem"))
	bool bSingleDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | Damage", meta = (EditCondition = "bActivateDamageSystem"))
	bool bDamagePerSecond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | Damage", meta = (EditCondition = "bActivateDamageSystem"))
	bool bDamageOverTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | Damage", meta = (EditCondition = "bDamageOverTime"))
	float TimeAmount_Damage;

	// Heal System
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | Heal", meta = (EditCondition = "!bActivateDamageSystem"))
	bool bActivateHealingSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | Heal", meta = (EditCondition = "bActivateHealingSystem"))
	float HealAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | Heal", meta = (EditCondition = "bActivateHealingSystem"))
	bool bHealOverTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | Heal", meta = (EditCondition = "bHealOverTime"))
	float TimeAmount_Healing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props | Itemdestruction")
	bool bDestroyAfterCollision;

	AMOGChar* MyChar;

	FTimerHandle OverTimeHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bIsInZone;
	bool bTimerActive;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void DisableTimer();

	void DestroyComponents();
};
