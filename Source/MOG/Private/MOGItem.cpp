// Fill out your copyright notice in the Description page of Project Settings.


#include "MOGItem.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MOGChar.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMOGItem::AMOGItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollsionSphere"));
	CollisionSphere->SetupAttachment(MeshComp);

	CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	StartEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("StartEffect"));
	StartEffect->SetupAttachment(MeshComp);

	bActiveAtOverlap = false;
	bActivateDamageSystem = false;
	bSingleDamage = false;
	bDestroyAfterCollision = false;
	bDamageOverTime = false;
	bDamagePerSecond = false;
	bIsInZone = false;
	bTimerActive = false;
	bAttachToPlayer = false;

	DamageToPlayer = 10.0f;
	DamageToAI = 10.0f;

	HealAmount = 10.0f;
}

// Called when the game starts or when spawned
void AMOGItem::BeginPlay()
{
	Super::BeginPlay();

	if (StartEffect != nullptr && !bActiveAtOverlap)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ItemSound, GetActorLocation());
	}

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AMOGItem::HandleBeginOverlap);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AMOGItem::HandleEndOverlap);
}

// Called every frame
void AMOGItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DeltaDamageToPlayer = DamageToPlayer * DeltaTime;
	float DeltaDamageToAI = DamageToAI * DeltaTime;

	float DeltaHealAmount = HealAmount * DeltaTime;

	float DeltaDoTDamage = (DamageToPlayer / TimeAmount_Damage) * DeltaTime;
	float DeltaHoT = (HealAmount / TimeAmount_Healing) * DeltaTime;

	// Over Time Functions
	if ((bActivateDamageSystem && bTimerActive) || bActivateHealingSystem && bTimerActive)
	{
		if (MyChar)
		{
			float& PlayerHeath = MyChar->CurrentHealth;

			if (bActivateDamageSystem)
			{
				PlayerHeath -= DeltaDoTDamage;
			} 
			
			if (bActivateHealingSystem)
			{
				PlayerHeath += DeltaHoT;
			}
		}
		else
		{
			return;
		}
	}

	//Drain Player health each Second
	if (bIsInZone && bActivateDamageSystem && bDamagePerSecond && StartEffect != nullptr)
	{
		if (MyChar)
		{
			float& PlayerHeath = MyChar->CurrentHealth;

			PlayerHeath -= DeltaDamageToPlayer;
		}
		// Damage AI Construct here
	}
}

void AMOGItem::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		bIsInZone = true;
		
		// Get Access to the Values of the overlapped Actor
		AMOGChar* Player = Cast<AMOGChar>(OtherActor);
		if (Player)
		{
			MyChar = Player;
		}

		if (bActiveAtOverlap)
		{
			if (MyChar)
			{
				if (OverlapEffect != nullptr)
				{
					if (bAttachToPlayer)
					{
						UGameplayStatics::SpawnEmitterAttached(OverlapEffect, MyChar->GetMesh(), FName("EffectSocket"));
					}
					else
					{
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapEffect, GetActorLocation());
					}
					UGameplayStatics::PlaySoundAtLocation(this, ItemSound, GetActorLocation());
				}
			}
		}

		// Damage every Collision
		if (bActivateDamageSystem && bSingleDamage)
		{
			UGameplayStatics::PlaySound2D(this, PlayerHitSound);

			if (MyChar)
			{
				MyChar->CurrentHealth -= DamageToPlayer;
			}
			// Damage AI Construct here

			if (bDestroyAfterCollision)
			{
				DestroyComponents();
			}
		}

		// Damage after Collision over Time
		if (bActivateDamageSystem && bDamageOverTime)
		{
			bTimerActive = true;

			// set Timer
			GetWorldTimerManager().SetTimer(OverTimeHandle, this, &AMOGItem::DisableTimer, TimeAmount_Damage, false);

			if (bDestroyAfterCollision)
			{
				DestroyComponents();
			}
		}

		// Heal after Collision over Time
		if (MyChar)
		{
			if (bActivateHealingSystem && bHealOverTime)
			{
				bTimerActive = true;

				GetWorldTimerManager().SetTimer(OverTimeHandle, this, &AMOGItem::DisableTimer, TimeAmount_Healing, false);

				if (bDestroyAfterCollision)
				{
					DestroyComponents();
				}
			}
		}
	}
}

void AMOGItem::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		bIsInZone = false;
	}
}

void AMOGItem::DisableTimer()
{
	bTimerActive = false;

	if (GetWorldTimerManager().IsTimerActive(OverTimeHandle))
	{
		GetWorldTimerManager().ClearTimer(OverTimeHandle);
	}
}

void AMOGItem::DestroyComponents()
{
	if (CollisionSphere)
	{
		CollisionSphere->DestroyComponent();
	}

	if (MeshComp)
	{
		MeshComp->DestroyComponent();
	}

	if (StartEffect)
	{
		StartEffect->DestroyComponent();
	}
}
