#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT01_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	class UUserWidget *EnemyHealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WEnemyHealthBar;


	// referenca na UMG asset u editoru
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> HUDOverlayAsset;
	
	// varijabla koja ce da drzi widget nakon sto je kreiran
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget *HUDOverlay;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WPauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget *PauseMenu;
	
	// indikator za prikaz pause menija
	bool bPauseMenuVisible;

	// prikazi/sakrij pause menu
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void DisplayPauseMenu();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void RemovePauseMenu();
	// ako je otvoren zatvori, ako je zatvoren otvori pause menu
	void TogglePauseMenu();

	// setuje odgovarajuci game mode i ponasanje misa
	void GameModeOnly();

	// indikator za prikaz enemyjevog health bara
	bool bEnemyHealthBarVisible;

	// prikazi/sakrij enemyjev health bar
	void DisplayEnemyHealthBar();
	void RemoveEnemyHealthBar();

	// lokacija enemyja
	FVector EnemyLocation;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};