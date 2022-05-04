// Fill out your copyright notice in the Description page of Project Settings.


#include "MOGEnemy.h"
#include "AIController.h"
#include "MOGChar.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet//KismetMathLibrary.h"
#include "MOGPlayerController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AMOGEnemy::AMOGEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
	AggroSphere->SetupAttachment(RootComponent);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(RootComponent);

	ChasingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ChasingSphere"));
	ChasingSphere->SetupAttachment(RootComponent);

	EnemyWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EnemyWeapon"));
	EnemyWeapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));

	bIsOverlappingCombatSphere = false;
	bIsAttacking = false;
	bIsDead = false;
	bDied = false;
	bIsHardAttack = false;
	bIsTimerActive = false;

	bInterpToPlayer = false;
	InterpSpeed = 10.0f;

	HitNumber = 0;
}

FRotator AMOGEnemy::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),Target);
	FRotator LookAtRotationYaw(0.0f, LookAtRotation.Yaw, 0.0f);

	return LookAtRotationYaw;
}

// Called when the game starts or when spawned
void AMOGEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AMOGEnemy::AggroOverlapBegin);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AMOGEnemy::CombatOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AMOGEnemy::CombatOverlapEnd);

	ChasingSphere->OnComponentBeginOverlap.AddDynamic(this, &AMOGEnemy::ChasingOverlapBegin);
	ChasingSphere->OnComponentEndOverlap.AddDynamic(this, &AMOGEnemy::ChasingOverlapEnd);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AMOGEnemy::WeaponOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AMOGEnemy::WeaponOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	EnemySpawnLocation = GetActorLocation();

	EnemyAnimInstance = GetMesh()->GetAnimInstance();
}

// Called every frame
void AMOGEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DeltaHealthRecovery = EnemyAttributes.EnemyHealthRecovery * DeltaTime;

	if (!bIsDead)
	{
		EnemyHealthSystem(DeltaHealthRecovery);
	}

	if (bInterpToPlayer && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}
}

void AMOGEnemy::MoveToTarget(AMOGChar* Target)
{
	if (AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(5.0f);

		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);
	}
}

void AMOGEnemy::GoBackToStartPosition(FVector StartPosition) const
{
	if (AIController)
	{
		AIController->MoveToLocation(StartPosition);
	}
}

void AMOGEnemy::AggroOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMOGChar* MyChar = Cast<AMOGChar>(OtherActor);
		if (MyChar)
		{
			CombatTarget = MyChar;

			MyChar->SetCombatTarget(this);
			MyChar->SetHasCombatTarget(true);

			MyChar->MyPlayerController->DisplayEnemyHealthbar();

			SetEnemyStatus(EEnemyStatus::EES_Chasing);
		}
	}
}

void AMOGEnemy::AggroOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		//
	}
}

void AMOGEnemy::CombatOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMOGChar* MyChar = Cast<AMOGChar>(OtherActor);
		if (MyChar)
		{
			bIsOverlappingCombatSphere = true;

			SetEnemyStatus(EEnemyStatus::EES_Combat);
		}
	}
}

void AMOGEnemy::ChasingOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*
	if (OtherActor)
	{
		AMOGChar* Player = Cast<AMOGChar>(OtherActor);
		if (Player)
		{
			MyChar = Player;
		}
	}
	*/
}

void AMOGEnemy::CombatOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMOGChar* MyChar = Cast<AMOGChar>(OtherActor);
		if (MyChar)
		{
			bIsOverlappingCombatSphere = false;
		}
	}
}

void AMOGEnemy::ChasingOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMOGChar* MyChar = Cast<AMOGChar>(OtherActor);
		if (MyChar)
		{
			if (MyChar->CombatTarget == this)
			{
				MyChar->SetCombatTarget(nullptr);
				MyChar->SetHasCombatTarget(false);
			}

			MyChar->MyPlayerController->HideEnemyHealthbar();

			CombatTarget = nullptr;

			SetEnemyStatus(EEnemyStatus::EES_Patroling);
		}
	}
}

void AMOGEnemy::EnemyHealthSystem(float RecoveryRate)
{
	if (EnemyAttributes.EnemyCurrentHealth <= 0.0f)
	{
		EnemyDeath();
	}

	// Recover Health over Time
	if (EnemyAttributes.EnemyCurrentHealth > EnemyAttributes.EnemyMaxHealth)
	{
		EnemyAttributes.EnemyCurrentHealth = EnemyAttributes.EnemyMaxHealth;
	}
	else
	{
		EnemyAttributes.EnemyCurrentHealth += RecoveryRate;
	}
}

void AMOGEnemy::EnemyDeath()
{
	SetInterpToPlayer(false);

	AIController->StopMovement();

	SetEnemyStatus(EEnemyStatus::EES_Dead);
}

void AMOGEnemy::AttackSystem()
{
	if (EnemyAnimInstance)
	{
		if (!bIsAttacking)
		{
			if (CombatTarget->bIsDead)
			{
				SetEnemyStatus(EEnemyStatus::EES_Patroling);
			}
			else
			{
				bIsAttacking = true;
				SetInterpToPlayer(true);

				int32 RandomNumber = FMath::RandRange(0, 3);

				EnemyAnimInstance->Montage_Play(Attack_Montage, 1.0f);

				if (RandomNumber == 3)
				{
					bIsHardAttack = true;
					EnemyAnimInstance->Montage_JumpToSection(FName("Attack1"), Attack_Montage);
				}
				else
				{
					if (bIsHardAttack)
					{
						bIsHardAttack = false;
					}
					EnemyAnimInstance->Montage_JumpToSection(FName("Attack2"), Attack_Montage);
				}
			}
		}
	}
}

void AMOGEnemy::SetInterpToPlayer(bool Interp)
{
	bInterpToPlayer = Interp;
}

void AMOGEnemy::WeaponOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMOGChar* MyChar = Cast<AMOGChar>(OtherActor);
		if (MyChar)
		{
			HitNumber += 1;
			if (HitNumber < 2)
			{
				DamageToPlayer(MyChar);

				const USkeletalMeshSocket* FXSocket = EnemyWeapon->GetSocketByName("FXSocket");
				if (FXSocket)
				{
					FVector SocketLocation = FXSocket->GetSocketLocation(EnemyWeapon);

					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MyChar->HitFX, SocketLocation);

					if (HitSound)
					{
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, SocketLocation);
					}
				}
			}
		}
	}
}

void AMOGEnemy::WeaponOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//
}

void AMOGEnemy::DamageToPlayer(AMOGChar* Player)
{
	float& PlayerHealth = Player->CurrentHealth;

	if (bIsHardAttack)
	{
		PlayerHealth -= EnemyAttributes.Damage * 1.5f;
	}
	else
	{
		PlayerHealth -= EnemyAttributes.Damage;
	}
}

void AMOGEnemy::ActivateCollsion()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMOGEnemy::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HitNumber = 0;
}

void AMOGEnemy::SetEnemyStatus(EEnemyStatus Status)
{
	EnemyStatus = Status;

	switch (EnemyStatus)
	{
	case EEnemyStatus::EES_Patroling:
		GetCharacterMovement()->MaxWalkSpeed = 200.0f;

		ActualLocation = GetActorLocation();

		if (ActualLocation != EnemySpawnLocation)
		{
			GoBackToStartPosition(EnemySpawnLocation);
		}
		break;
	case EEnemyStatus::EES_Chasing:
		GetCharacterMovement()->MaxWalkSpeed = 360.0f;

		MoveToTarget(CombatTarget);
		break;
	case EEnemyStatus::EES_Combat:
		AttackSystem();
		break;
	case EEnemyStatus::EES_Dead:
		bIsDead = true;
		break;
	default:
		break;
	}
}
