// Fill out your copyright notice in the Description page of Project Settings.


#include "MOGAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"
#include "MOGChar.h"

void UMOGAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			MyChar = Cast<AMOGChar>(Pawn);
		}
	}
}

void UMOGAnimInstance::UpdateAnimation()
{
	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0f);
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
	}
}
