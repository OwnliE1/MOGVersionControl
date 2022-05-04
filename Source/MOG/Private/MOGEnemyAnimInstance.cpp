// Fill out your copyright notice in the Description page of Project Settings.


#include "MOGEnemyAnimInstance.h"
#include "MOGEnemy.h"
#include "GameFramework/PawnMovementComponent.h"

void UMOGEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			MyEnemy = Cast<AMOGEnemy>(Pawn);
		}
	}
}

void UMOGEnemyAnimInstance::UpdateAnimation()
{
	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0f);
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
	}
}
