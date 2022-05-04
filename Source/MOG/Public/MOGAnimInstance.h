// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MOGAnimInstance.generated.h"

class AMOGChar;

/**
 * 
 */
UCLASS()
class MOG_API UMOGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	virtual void NativeInitializeAnimation() override;

	APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Props")
	AMOGChar* MyChar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Props")
	bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Props")
	float MovementSpeed;

	UFUNCTION(BlueprintCallable)
	void UpdateAnimation();

};
