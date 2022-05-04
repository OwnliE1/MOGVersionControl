// Fill out your copyright notice in the Description page of Project Settings.


#include "MOGWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "MOGChar.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "MOGEnemy.h"

// Sets default values
AMOGWeapon::AMOGWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	RootComponent = SkeletalMeshComp;

	PickupCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	PickupCollision->SetupAttachment(SkeletalMeshComp);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(SkeletalMeshComp);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	MagicFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MagicFX"));
	MagicFX->SetupAttachment(SkeletalMeshComp);

	bKeepFXAfterPickup = false;

	WeaponState = EWeaponState::EWS_Pickup;

	LightAttackStamina = 15.0f;
	HardAttackStamina = 30.0f;

	LightAttackSpeed = 1.0f;
	HardAttackSpeed = 1.0f;

	HitNumber = 0;
}

// Called when the game starts or when spawned
void AMOGWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AMOGWeapon::HandleBeginOverlap);
	PickupCollision->OnComponentEndOverlap.AddDynamic(this, &AMOGWeapon::HandleEndOverlap);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AMOGWeapon::CombatOverlapBegin);
}

// Called every frame
void AMOGWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMOGWeapon::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (WeaponState == EWeaponState::EWS_Pickup)
		{
			AMOGChar* Player = Cast<AMOGChar>(OtherActor);
			if (Player)
			{
				MyChar = Player;
				MyChar->SetOverlappingWeapon(this);
			}
		}
	}
}

void AMOGWeapon::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (MyChar)
		{
			MyChar->SetOverlappingWeapon(nullptr);
		}
	}
}

void AMOGWeapon::CombatOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMOGEnemy* Enemy = Cast<AMOGEnemy>(OtherActor);
		if (Enemy)
		{
			HitNumber += 1;
			if (HitNumber < 2)
			{
				AITakeDamage(Enemy);

				if (Enemy->HitFX)
				{
					const USkeletalMeshSocket* FXSocket = SkeletalMeshComp->GetSocketByName("FXSocket");
					if (FXSocket)
					{
						FVector SocketLocation = FXSocket->GetSocketLocation(SkeletalMeshComp);
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitFX, SocketLocation);

						if (MyChar->bIsLightAttack)
						{
							UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound_Light, SocketLocation);
						}

						if (MyChar->bIsHardAttack)
						{
							UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound_Hard, SocketLocation);
						}
					}
				}
			}
		}
	}
}

void AMOGWeapon::Equip(AMOGChar* PlayerChar)
{
	if (PlayerChar)
	{
		UGameplayStatics::PlaySound2D(this, PickupSound);

		// ignore collision on camera
		SkeletalMeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		// ignore Player itself
		SkeletalMeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		// disable Simulating Physics if enabled
		SkeletalMeshComp->SetSimulatePhysics(false);

		PickupCollision->DestroyComponent();

		ChooseAndAttach(PlayerChar);
		
		PlayerChar->SetWeapon(this);

		if (!bKeepFXAfterPickup)
		{
			MagicFX->DestroyComponent();
		}
	}
}

void AMOGWeapon::ChooseAndAttach(AMOGChar* Player)
{
	if (Player)
	{
		const USkeletalMeshSocket* OneHandedSocket = Player->GetMesh()->GetSocketByName("OneHandedWeapon");
		if (OneHandedSocket)
		{
			OneHandedSocket->AttachActor(this, Player->GetMesh());
		}
	}
}

void AMOGWeapon::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMOGWeapon::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HitNumber = 0;
}

void AMOGWeapon::AITakeDamage(AMOGEnemy* MyEnemy)
{
	float& EnemyHealth = MyEnemy->EnemyAttributes.EnemyCurrentHealth;

	float DamageAI = WeaponAttributes.WeaponDamage;

	if (MyChar->bIsHardAttack)
	{
		DamageAI *= 1.5f;
	}

	EnemyHealth -= DamageAI;
}
