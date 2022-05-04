// Fill out your copyright notice in the Description page of Project Settings.


#include "MOGPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMOGPlayerController::DisplayEnemyHealthbar()
{
	if (EnemyHealthbar)
	{
		bEnemyHealthbarVisible = true;
		EnemyHealthbar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMOGPlayerController::HideEnemyHealthbar()
{

	if (EnemyHealthbar)
	{
		bEnemyHealthbarVisible = false;
		EnemyHealthbar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMOGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
		HUDOverlay->AddToViewport();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Red, TEXT("Set HUDOverlayAsset in PlayerController !"), true, FVector2D(2.0f, 2.0f));
		return;
	}

	if (WEnemyHealthbar)
	{
		EnemyHealthbar = CreateWidget<UUserWidget>(this, WEnemyHealthbar);
		EnemyHealthbar->AddToViewport();
		EnemyHealthbar->SetVisibility(ESlateVisibility::Hidden);

		FVector2D AligmentVector(0.5f, 7.0f);
		EnemyHealthbar->SetAlignmentInViewport(AligmentVector);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Red, TEXT("Set EnemyHealthbar in PlayerController !"), true, FVector2D(2.0f, 2.0f));
		return;
	}
}

void AMOGPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyHealthbar)
	{
		FVector2D PositionInViewport;
		FVector2D SizeInViewport(250.0f, 25.0f);

		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);

		EnemyHealthbar->SetPositionInViewport(PositionInViewport);
		EnemyHealthbar->SetDesiredSizeInViewport(SizeInViewport);
	}
}
