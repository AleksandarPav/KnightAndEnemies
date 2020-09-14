#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"


void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// ako je odabran u BP
	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset); // prima PlayerController, pa UClass
		if (HUDOverlay)
		{
			// dodaj na viewport
			HUDOverlay->AddToViewport();
			// neka bude vidljivo
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}

	// sve isto kao gore, samo sad za Enemyjev health bar
	if (WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthBar)
		{
			EnemyHealthBar->AddToViewport();
			// hocemo da se ne vidi (osim onda kad treba da se vidi)
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);

			// poravnanje u viewportu...
			FVector2D Alignment(0.f, 0.f);
			EnemyHealthBar->SetAlignmentInViewport(Alignment);
		}
	}

	// sve isto kao gore, samo sad za Pause menu
	if (WPauseMenu)
	{
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
		if (PauseMenu)
		{
			PauseMenu->AddToViewport();
			// hocemo da se ne vidi (osim onda kad treba da se vidi)
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AMainPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyHealthBar)
	{
		// za poziciju Enemyjevog HealthBara, treba nam 2D vektor
		FVector2D PositionInViewport;
		// projektuje 3D lokaciju u svetu na 2D vektor ekrana i tu projekciju
		// smesti u drugi argument f-je
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		// da bude malo iznad enemyja
		PositionInViewport.Y -= 50.f;

		// velicina Enemyjevog HealthBara, 300 u sirinu i 25 u visinu
		FVector2D SizeInViewport(300.f, 25.f);

		// postavi poziciju i velicinu u viewport
		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}
}

void AMainPlayerController::DisplayPauseMenu_Implementation()
{
	// postavi pause menu da bude vidljiv
	if (PauseMenu)
	{
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);

		// SetInputMode prima FInputMode strukturu, tako da kreirana jedna vrsta te strukture
		FInputModeGameAndUI InputModeGameAndUI;
		// zamrzni sve kad se pojavi pause menu, moze samo kursor da se krece
		SetInputMode(InputModeGameAndUI);
		// ali kursor se jos uvek ne vidi, tako da:
		bShowMouseCursor = true;
	}
}

void AMainPlayerController::RemovePauseMenu_Implementation()
{
	if (PauseMenu)
	{
		bPauseMenuVisible = false;

		// setuj odgovarajuci game mode
		GameModeOnly();
		
		// skloni kursor kad smo ukloni pause menu
		bShowMouseCursor = false;
	}
}

void AMainPlayerController::TogglePauseMenu()
{
	// ako je otvoren zatvori i obratno
	if (bPauseMenuVisible)
		RemovePauseMenu();
	else
		DisplayPauseMenu();
}

void AMainPlayerController::GameModeOnly()
{
	// SetInputMode prima FInputMode strukturu, tako da kreirana jedna vrsta te strukture
	FInputModeGameOnly InputModeGameOnly;
	// aktivna samo igra kad smo sklonili pause menu
	SetInputMode(InputModeGameOnly);
}