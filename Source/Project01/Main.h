// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

/*
	ne mora da se kreira mesh - vec kreiran u ACharacter, tako da se nasledjuje (SkeletalMesh konkretno);
*/

// enum za nacin kretanja; moze da se koristi i u Blueprints;
// kad se koristi UENUM, mora i UMETA; MAX je praksa da se stavlja na kraju, posle poslednjeg enuma;
UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprint UMETA(DisplayName = "Sprint"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};
// enumeracija za status stamina bara; normal je kad sprinta i jos nije dostigao kriticnu tacku,
// below minimum je kad predje kriticnu tacku i stamina bar treba da promeni boju,
// exhausted je kad dodje do nule i opet promeni boju,
// exhausted recovering je kad krene da se oporavlja od nule ali i dalje ne moze da sprinta dok ne predje kriticnu tacku;
UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),
	ESS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class PROJECT01_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	// loadovanje igre kad nema promene nivoa
	void LoadGameNoSwitchLevel();

	// f-ja za proveru da li je sve ispunjeno za dozvolu kretanja
	bool CanMove(float Value);

	// called for Yaw rotation
	void Turn(float Value);
	// called for Pitch rotation
	void LookUp(float Value);

	// indikator da li je Escape pritisnut ili ne i odgovarajuce f-je
	bool bESCDown;
	void ESCDown();
	void ESCUp();

	// varijabla za ItemStorage koji cuva podatke o oruzju
	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TSubclassOf<class AItemStorage> WeaponStorage;

	UFUNCTION(BlueprintCallable)
	void SaveGame();
	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	// f-ja za menjanje levela
	void SwitchLevel(FName LevelName);

	// indikatori kretanja
	bool bMovingForward;
	bool bMovingRight;
	
	// indikator da li je ziv
	bool bDead;

	// f-ja koja se trigeruje na kraju animacije za smrt
	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	// override ACharacterove f-je Jump()
	virtual void Jump() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector CombatTargetLocation;

	// varijabla koja nam sluzi kao veza do MainPlayerControllerovih f-ja za prikaz/skrivanje EnemyHealthBara
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController *MainPlayerController;

	// indikator da li ima combat target
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	// seter indikatora da li Main ima CombatTarget
	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	// f-ja nasledjena iz Actora; parametri iskopirani
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	// interpolating to the enemy
	float InterpSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);

	// target ka kojem interpoliramo Maina
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy *CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy *Target) { CombatTarget = Target; }

	// vraca ciljanu rotaciju prilikom interpolacije Mainove rotacije ka enemyju,
	// koji se nalazi na lokaciji FVector Target
	FRotator GetLookAtRotationYaw(FVector Target);

	// zvuk koji ce da se cuje kad Main bude hitovan od strane enemyja
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue *HitSound;

	// particle effect kad enemy sibne igraca
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem *HitParticles;

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	// promenljiva za AnimMontage koji smo kreirali, pod imenom CombatMontage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage *CombatMontage;

	// indikator da li je igrac u rezimu napadanja
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	// f-ja za aktiviranje animacije za napadanje
	void Attack();
	
	// regulise zavrsetak napada
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	// weapon koji ce biti dodeljen ovom karakteru
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AWeapon *EquippedWeapon;

	// seter za weapon
	FORCEINLINE void SetEquippedWeapon(AWeapon *WeaponToSet) { EquippedWeapon = WeaponToSet; }

	// item s kojim se trenutno preklapamo; da pruzimo mogucnost igracu da ne prihvati oruzje;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem *ActiveOverlappingItem;

	// seter za preklapajuci item;
	FORCEINLINE void SetActiveOverlappingItem(AItem *Item) { ActiveOverlappingItem = Item; }

	// indikator da li je LeftMouseButton pritisnut ili ne i odgovarajuce f-je
	bool bLMBDown;
	void LMBDown();
	void LMBUp();

	// vektor u kojem ce biti lokacije pickup itema
	TArray<FVector> PickupLocations;

	// f-ja koja ce da iscrta debug sferu tamo gde je bio pickup kojeg vise nema
	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	// status kretanja (normal, sprint)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	// setuj movement status na prosledjenu vrednost i promeni brzinu kretanja;
	void SetMovementStatus(EMovementStatus);

	// promenljiva za status stamina bara
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	// setuj stamina status na prosledjenu vrednost;
	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	// brzina kojom ce stamina da se smanjuje dok sprinta
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;
	// minim stamine s kojom moze da se sprinta
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	// brzina za trcanje i sprintanje; inicijalizovani u konstruktoru;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	// indikator da li je pritisnut Shift
	bool bShiftKeyDown;
	// omoguci sprint
	void ShiftKeyDown();
	// onemoguci sprint
	void ShiftKeyUp();

	// moguce i category bez navodnika, sam ce konvertovati u string;
	// hocemo da ovo bude pristupacno samo iz BP that contains it, ne iz ostalih, zato PrivateAccess;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	// forward declare it - stavljanje class ispred; da se intelisense ne buni;
	// camera boom positioning the camera behind the player;
	class USpringArmComponent *CameraBoom;
	
	// kamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent *FollowCamera;

	// turn rate to scale turning functions for the camera (desno-levo);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	// turn rate to scale turning functions for the camera (gore-dole);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;


	/*
	
		Player Statistics

	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	// smanjivanje healtha za amount;
	void DecrementHealth(float Amount);
	// umiranje
	void Die();

	// uvecavanja coina za amount;
	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount);
	// uvecavanje healtha za amount
	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	// kad ubije jednom enemyja, updateuj na drugog ako treba
	void UpdateCombatTarget();

	// drugi parametar za GetOverlappingActors f-ju;
	// osigurava da vrati samo one actore koji su navedenog tipa;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// called for forwards/backwards input;
	void MoveForward(float Value);
	// called for side to side input;
	void MoveRight(float Value);

	// called via input to turn at a given rate;
	// @param Rate - normalized rate [0-1];
	void TurnAtRate(float Rate);
	// called via input to look up/down at a given rate;
	// @param Rate - normalized rate [0-1];
	void LookUpAtRate(float Rate);

	// geteri za spring arm i kameru;
	FORCEINLINE class USpringArmComponent *GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent *GetFollowCamera() const { return FollowCamera; }
};
