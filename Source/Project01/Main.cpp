// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
// header za spring arm;
#include "GameFramework/SpringArmComponent.h"
// header za kameru;
#include "Camera/CameraComponent.h"
// header za controller;
#include "GameFramework/Controller.h"
// header za GetWorld(); treba nam za DeltaTime (sekunde izmedju dva frejma);
#include "Engine/World.h"
// header za player input component
#include "Components/InputComponent.h"
// header za CapsuleComponent;
#include "Components/CapsuleComponent.h"
// header za CharacterMovementComponent;
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "SaveGame01.h"
#include "ItemStorage.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// create camera boom (pulls towards the player if there's a collision);
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; // camera follows at this distance;
	// ako se desi input koji rotira controllera, nateraj spring arm da prati controller rotation;
	CameraBoom->bUsePawnControlRotation = true; // rotate arm based on controller;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// attach camera to the boom;
	// socket je nesto sto omogucava assetima da se stvari attachuju za njih;
	// npr. hocemo da junak moze da drzi mac (treba nam da mac bude attached to the character's hand):
	// stavi se socket na hand bone od karakterovog skeletona i time dobijamo funkcionalnost attachovanja
	// meshova to the socket; spring arm ima vec default socket name i taj socket se nalazi na kraju sipke za kameru;
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// ne zelimo da se kamera rotira sa inputom; treba nam fiksna na kraj spring arma;
	// neka se spring arm okrece s inputom, ali ne i kamera;
	FollowCamera->bUsePawnControlRotation = false;

	// inicijalizuj velicinu kapusule za collisions;
	// setovao kapsulu rucno u Main_BP, procitao vrednosti i zaokruzio;
	GetCapsuleComponent()->InitCapsuleSize(44.f, 107.f);

	// inicijalizacija rate-a za rotaciju oko karaktera;
	BaseTurnRate = 40.f;
	BaseLookUpRate = 40.f;

	// kad se okrece mis (ili strelice levo-desno), ne zelimo da se i karakter okrece, nego da stoji u mesu;
	// dakle kamera ok, ali ne i igrac;
	bUseControllerRotationYaw = false;
	// oko ostalih osa se ionako ne okrece, ali za svaki slucaj:
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// da se karakter automatski okrene u smeru u kojem se krece:
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// rotation rate za okretanje karaktera u smeru u kojem se krece;
	// hocemo da se samo yaw rotation prilagodjava pokretu, ne da se okrece ka smeru u kojem skace i sl.
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	// brzina kojom ce da skace;
	GetCharacterMovement()->JumpZVelocity = 650.f;
	// ogranici mu mobilnost dok je u vazduhu;
	GetCharacterMovement()->AirControl = 0.2;

	// inicijalizacija Player Stats
	MaxHealth = 100.f;
	Health = 100.f;
	MaxStamina = 150.f;
	Stamina = 150.f;
	Coins = 0;

	// inicijalizacija brzina za trcanje i sprint
	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;

	// default vrednost za pritisnutost Shift dugmeta;
	bShiftKeyDown = false;
	// default vrednost za pritisnutost LeftMouse tastera;
	bLMBDown = false;

	// inicijalizacija enuma; po default trci normalno (ne sprinta) i stamina je u normalnom rezimu (nije exhausted ili bilo sta);
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	// inicijalizacija drain rate na 25 jedinica stamine po sekundi
	StaminaDrainRate = 15.f;
	// minimalna stamina potrebna za sprint
	MinSprintStamina = 30.f;

	// default vrednost za rezim napadanja
	bAttacking = false;

	// default parametri za interpolaciju Maina ka Enemyju
	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	// default indikator da li ima CombatTarget
	bHasCombatTarget = false;

	// default vrednost za da l' je mrtav
	bDead = false;

	// default indikatori za kretanje
	bMovingForward = false;
	bMovingRight = false;

	// default pritisnutost Escape dugmeta
	bESCDown = false;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();
	
	//// nacrtaj zelenu sferu oko actora na lokaciji actora + 175 u visinu,
	//// radijusa 45, 8 segmenata, zelene boje, 10 sekundi trajanja i thicknessa 0.5;
	//// uglavnom pratio sugerisanje intelisensa za parametre;
	//DrawDebugSphere(GetWorld(), GetActorLocation() + FVector(0.f, 0.f, 175.f), 45.f, 8, FColor::Green, false, 10.f, (uint8)'\000', .5f);

	// AController koji vrati GetController() castuj u AMainPlayerController
	MainPlayerController = Cast<AMainPlayerController>(GetController());

	// ako je poceo BeginPlay() i nalazimo se u prvom levelu, ne zelimo loadovanje igre, neka krene ispocetka
	FString Map = GetWorld()->GetMapName();
	Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (Map != "SunTemple")
	{
		// loaduj podatke za igru, verzija bez promene levela
		LoadGameNoSwitchLevel();

		// postavi game mode na GameModeOnly da ne bi mis i dalje kliktao van igre
		if (MainPlayerController)
			MainPlayerController->GameModeOnly();
	}

	// postavi game mode na GameModeOnly da ne bi mis i dalje kliktao van igre
	if (MainPlayerController)
		MainPlayerController->GameModeOnly();

	//// loaduj podatke za igru, verzija bez promene levela
	//LoadGameNoSwitchLevel();

	//// postavi game mode na GameModeOnly da ne bi mis i dalje kliktao van igre
	//if (MainPlayerController)
	//	MainPlayerController->GameModeOnly();
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ako mrtav, ne zanima nas sta je u ostatku f-je
	if (MovementStatus == EMovementStatus::EMS_Dead)
		return;

	// koliko ce stamina da se promeni tokom jednog frejma (bilo za drain bilo za recovery)
	float DeltaStamina = StaminaDrainRate * DeltaTime;

	// svi moguci slucajevi stamina statusa
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal: // normalan (plavi) rezim

		if (bShiftKeyDown) // ako pritisnut Shift
		{
			if (bMovingForward || bMovingRight) // ako se Main krece
			{
				// ako ce gi smanjis na manje od Min, promeni mu status na BelowMinimum;
				if (Stamina - DeltaStamina <= MinSprintStamina)
					SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);

				// oduzmi od stamine delta staminu
				Stamina -= DeltaStamina;

				// s obzirom da je pritisnut Shift i u normalnom smo rezimu, slobodan si sprintati
				SetMovementStatus(EMovementStatus::EMS_Sprint);
			}
			else // ako se ne krece, stavi ga na Normal
				SetMovementStatus(EMovementStatus::EMS_Normal);
				
		}
		else // ako nije pritisnut Shift
		{
			// uvecaj staminu, jer smo u normalnom rezimu
			Stamina += DeltaStamina;
			// ako presao granicu maks stamine, zakucaj ga na maks
			if (Stamina > MaxStamina)
				Stamina = MaxStamina;

			// nije pritisnut Shift i nalazimo se u Normal rezimu, tako da brzina je normalna a ne sprint
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}

		break;

	case EStaminaStatus::ESS_BelowMinimum: // ispod minimuma (zuti) rezim

		if (bShiftKeyDown) // pritisnut Shift
		{
			if (bMovingForward || bMovingRight) // ako se krece
			{
				// smanji staminu
				Stamina -= DeltaStamina;
				// ako je otisla do kurca skroz, postavi je na nulu, promeni rezim stamine u exhausted
				// i movement status ne moze da sprinta nego mora normal sad da ide
				if (Stamina < 0.f)
				{
					Stamina = 0.f;
					SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
				else // ako je stamina i dalje iznad nule, samo setuj MovementStatus da moze da sprinta jos uvek
					SetMovementStatus(EMovementStatus::EMS_Sprint);
			}
			else // ako se ne krece, stavi ga na normal
				SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		else // nije pritisnut Shift
		{
			// uvecaj staminu, nije pritisnut Shift
			Stamina += DeltaStamina;
			// ako presla granicu min sprinta, postavi je u normalan rezim
			if (Stamina >= MinSprintStamina)
				SetStaminaStatus(EStaminaStatus::ESS_Normal);

			// kretanje nije sprint jer nije pritisnut Shift
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}

		break;

	case EStaminaStatus::ESS_Exhausted: // iscprljen (narandzasti) rezim

		// ako je pritisnut Shift dok je exhausted, drzi staminu na nuli, mora da pusti da mi se recoverovao
		if (bShiftKeyDown)
			Stamina = 0.f;
		else // nije pritisnut Shift
		{
			// moze da se recoveruje
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}

		// kretanje nije sprint jer je u exhausted rezimu
		SetMovementStatus(EMovementStatus::EMS_Normal);

		break;

	case EStaminaStatus::ESS_ExhaustedRecovering: // exhausted, ali moze da se recoveruje

		// ako je stamina kad se poveca veca od min sprint stamine, vrati se u normalan (plavi) rezim
		Stamina += DeltaStamina;
		if (Stamina >= MinSprintStamina)
			SetStaminaStatus(EStaminaStatus::ESS_Normal);

		// posto smo exhausted recovering, ne moze da sprinta
		SetMovementStatus(EMovementStatus::EMS_Normal);

		break;

	default:
		;
	}

	// ako je indikator za interpoliranje true i CombatTarget nije nullptr
	if (bInterpToEnemy && CombatTarget)
	{
		// nadji ciljanu rotaciju
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		// interpoliraj rotaciju; prima trenutnu, ciljanu, DeltaTime i brzinu interpolacije koju smo vec definisali;
		// vraca rotaciju za trenutni frejm, omogucena smooth tranzicija
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

	if (CombatTarget)
	{
		// updateuj lokaciju enemyja
		CombatTargetLocation = CombatTarget->GetActorLocation();
		// updateuj lokaciju enemyja i u MainPlayerControlleru, zbog HealthBara
		if (MainPlayerController)
			MainPlayerController->EnemyLocation = CombatTargetLocation;
	}
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{	
	// vraca ciljanu rotaciju prilikom interpolacije; zanima nas samo Yaw;
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	return FRotator(0.f, LookAtRotation.Yaw, 0.f);
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// proverava da li je input validan; ako vrati false, zadrzava kod i ne nastavlja dok input nije validan;
	check(PlayerInputComponent);

	// bind action za skok; prima ime actiona koje smo stavili u ProjectSettings u UE,
	// enum za event (kad se pritisne dugme definisano u ProjectSettings - SpaceBar), ptr na objekat, ptr na f-ju;
	// Character ima built in f-ju za skok;
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AMain::Jump);
	// bind action za prestanak skoka; enum i ne mora sa scope operatorom; Character ima ugradjenu f-ju za prestanak skoka;
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// bind action za sprint; poziva f-je ShiftKeyDown()/ShiftKeyUp() koje smo kreirali;
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AMain::ShiftKeyDown);
	// bind action za prestanak sprinta;
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	// bind action za LMB; poziva f-je LMBDown()/LMBUp() koje smo kreirali;
	PlayerInputComponent->BindAction("LMB", EInputEvent::IE_Pressed, this, &AMain::LMBDown);
	// bind action za prestanak LMB;
	PlayerInputComponent->BindAction("LMB", EInputEvent::IE_Released, this, &AMain::LMBUp);

	// bind axis for move forward; prima ime ose kakvo smo dali u ProjectSettings u UE, ptr na objekat, ptr f-je koja vrsi
	// potrebnu funkcionalnost za tu osu;
	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	// bind axis for move right;
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	// bind axis za turn misem;
	PlayerInputComponent->BindAxis("Turn", this, &AMain::Turn);
	// bind axis za look up/down misem;
	PlayerInputComponent->BindAxis("LookUp", this, &AMain::LookUp);

	// bind axis za turn pomocu strelica sa tastature (left/right);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	// bind axis za look up/down pomocu strelica sa tastature (up/down);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);

	// bind action za ESC; poziva f-je ESCDown()/ESCUp() koje smo kreirali;
	PlayerInputComponent->BindAction("ESC", EInputEvent::IE_Pressed, this, &AMain::ESCDown);
	// bind action za prestanak ESC;
	PlayerInputComponent->BindAction("ESC", EInputEvent::IE_Released, this, &AMain::ESCUp);
}


// prilikom kretanja, hocemo da, ako okrecemo misa oko karaktera i okrenemo kameru ka njegovoj faci recimo,
// kad pritisnemo W, da krene u smeru u kojem gleda kamera, a ne na koji gleda mesh (sto je ka kameri u slucaju da
// okrenemo kameru za 180); uglavnom, hocemo da se karakter okrene i hoda ka smeru u na koji kamera gleda, a ne samo
// da hoda u smeru na koji gleda mesh; isto vazi i ako se pritisne A/D - levo/desno u odnosu na kameru, a ne u odnosu na junaka!!!!!

// called for forwards/backwards input;
void AMain::MoveForward(float Value)
{
	// svaki put kad se pozove f-ja (u svakom frejmu), postavi na false;
	// pa ako se pritisne dugme, dole se setuje na true
	bMovingForward = false;

	if (CanMove(Value))
	{
		// prvo hocemo da find out which way is FORWARD;
		// f-ja koja vraca rotator koji daje direction the controller is facing in this frame;
		const FRotator Rotation = Controller->GetControlRotation();
		// interesuje nas samo Yaw - ugao u horizontalnoj ravni, zabole me za Pitch i za Roll;
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// sad nam treba FORWARD vector;
		// dobijamo FORWARD vector based on yawrotation;
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // kreira FRotation matricu
		// (nije vektor, ne moze direction direktno da se inicijalizuje) iz YawRotation; matrica ima UnitAxis (jedinicni vektor) kojoj
		// se moze pristupiti i jedna od njih je X (FORWARD pravac);
		// kad god imamo rotation matrix, imamo informaciju o XYZ osama te rotacije u prostoru;
		// u sustini, bilo koje rotirano telo u prostoru ima svoju XYZ komponentu - ovo je nacin da im se pristupi;
		// controller prati kameru, tako da FORWARD vektor controllera odgovara smeru na koji gleda kamera;
		// dakle treba nam kretanje u tom smeru;
		AddMovementInput(Direction, Value);

		// indikator za kretanje: true
		bMovingForward = true;
	}

}

// called for side to side input;
void AMain::MoveRight(float Value)
{
	// svaki put kad se pozove f-ja (u svakom frejmu), postavi na false;
	// pa ako se pritisne dugme, dole se setuje na true
	bMovingRight = false;

	if (CanMove(Value))
	{
		// prvo hocemo da find out which way is RIGHT;
		// f-ja koja vraca rotator koji daje direction the controller is facing in this frame;
		const FRotator Rotation = Controller->GetControlRotation();
		// interesuje nas samo Yaw - ugao u horizontalnoj ravni, zabole me za Pitch i za Roll;
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// sad nam treba RIGHT vector;
		// dobijamo RIGHT vector based on yawrotation;
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); // kreira FRotation matricu
		// (nije vektor, ne moze direction direktno da se inicijalizuje) iz YawRotation; matrica ima UnitAxis (jedinicni vektor) kojoj
		// se moze pristupiti i jedna od njih je Y (RIGHT pravac);
		// kad god imamo rotation matrix, imamo informaciju o XYZ osama te rotacije u prostoru;
		// u sustini, bilo koje rotirano telo u prostoru ima svoju XYZ komponentu - ovo je nacin da im se pristupi;
		// controller prati kameru, tako da RIGHT vektor controllera odgovara levom/desnom smeru u odnosu na kameru (ne na mesh/junaka);
		// dakle treba nam kretanje u tom smeru;
		AddMovementInput(Direction, Value);

		// indikator za kretanje: true
		bMovingRight = true;
	}
}


// TurnAtRate i LookUpRate uzmu input sa left/right keypresses za rotaciju i prosledjuju u controller rotation

// called via input to turn at a given rate;
// @param Rate - normalized rate [0-1];
void AMain::TurnAtRate(float Rate)
{
	// BaseTurnRate inicijalizovan na 65 - znaci 65 stepeni/s;
	// da bi se dobio ugao, pomnozi se 65 stepeni/s sa DeltaTime (maaaaali broj sekundi izmedju dva frejma) i dobiju se stepeni
	// za koje se treba rotirati u horizontalnoj ravni (Yaw ugao) u jednom frejmu;
	// sve se to pomnozi sa Rate koji je bezdimenzioni (bice -1, 0 ili 1);
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

// called via input to look up/down at a given rate;
// @param Rate - normalized rate [0-1];
void AMain::LookUpAtRate(float Rate)
{
	// BaseLookUpRate inicijalizovan na 65 - znaci 65 stepeni/s;
	// da bi se dobio ugao, pomnozi se 65 stepeni/s sa DeltaTime (maaaaali broj sekundi izmedju dva frejma) i dobiju se stepeni
	// za koje se treba rotirati u ravni gore-dole (Pitch ugao) u jednom frejmu;
	// sve se to pomnozi sa Rate koji je bezdimenzioni (bice -1, 0 ili 1);
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::DecrementHealth(float Amount)
{
	// ako se skida vise nego sto ostalo healtha, dumri odmah;
	if (Amount >= Health)
	{
		Health = 0;
		Die();
	}
	// ako skidas manje nego ostalo healtha, smanji za Amount;
	else
		Health -= Amount;
}

void AMain::Die()
{
	// ako nikakav ptr nije nullptr, pusti animaciju;
	// u CombatMontage animaciji u UE, section smo nazvali Death
	UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}

	// crko
	SetMovementStatus(EMovementStatus::EMS_Dead);
	bDead = true;
}

void AMain::IncrementCoins(int32 Amount)
{
	// uvecaj coine za amount;
	Coins += Amount;
}


void AMain::IncrementHealth(float Amount)
{
	if (Health + Amount >= MaxHealth)
		Health = MaxHealth;
	else
		Health += Amount;
}

void AMain::SetMovementStatus(EMovementStatus Status)
{
	// postavi MovementStatus na prosledjeni;
	// MaxWalkSpeed je sadrzana u GetCharacterMovement() i odredjuje brzinu igraca;
	MovementStatus = Status;
	switch (MovementStatus)
	{
	case EMovementStatus::EMS_Normal:
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		break;
	case EMovementStatus::EMS_Sprint:
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
		break;
	default:
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		break;
	}
}

void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void AMain::ShowPickupLocations()
{
	for (FVector& Location : PickupLocations)
		// nacrtaj zelenu sferu oko actora na lokaciji actora + 75 u visinu,
		// radijusa 25, 8 segmenata, zelene boje, 10 sekundi trajanja i thicknessa 0.5;
		// uglavnom pratio sugerisanje intelisensa za parametre;
		DrawDebugSphere(GetWorld(), Location + FVector(0.f, 0.f, 75.f), 25.f, 8, FColor::Green, false, 10.f, (uint8)'\000', .5f);
}

// kad LMB pritisnut, indikator true
void AMain::LMBDown()
{
	// ako mrtav, izadji iz f-je
	if (MovementStatus == EMovementStatus::EMS_Dead)
		return;

	bLMBDown = true;

	// ako je aktivan pause menu, izadji iz f-je
	if (MainPlayerController)
		if (MainPlayerController->bPauseMenuVisible)
			return;

	// ako se overlapuje s nekim itemom, castuj item u Weapon
	if (ActiveOverlappingItem)
	{
		// ako cast bio uspesan, znaci da item bas jeste weapon i poziva se equip da se igrac opremi oruzjem
		AWeapon *Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			// kad je igrac opremljen oruzjem, ActiveOverlappingItem moze da postane nullptr
			SetActiveOverlappingItem(nullptr);
		}
	}
	// nema overlapovanja ni sa kim, znaci LMBDown je znak za napad
	else if (EquippedWeapon)
		Attack();
}

// kad LMB nije pritisnut, indikator false
void AMain::LMBUp()
{
	bLMBDown = false;
}

void AMain::Attack()
{
	// mozes zapoceti nov napad samo ako je bAttacking trenutno false i Main je ziv
	// (da bi se sprecilo da na svaki klik zapocne napad ispocetka)
	if (!bAttacking && (MovementStatus != EMovementStatus::EMS_Dead))
	{
		// indikator za napadanje postavi na true
		bAttacking = true;
		// setuj interpolaciju ka enemyju kad krene napad na true
		SetInterpToEnemy(true);

		// getujemo anim instance
		UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
		// ako AnimInstance i CombatMontage nisu nullptr, zovemo f-ju pomocu koje se play montage
		if (AnimInstance && CombatMontage)
		{
			// vratice random 0 ili 1
			int32 SectionRand = FMath::RandRange(0, 1);
			FName Section;
			// ako vrati 0, biramo section Attack_1; ako vrati 1, biramo section Attack_2;
			switch (SectionRand)
			{
			case 0:
				Section = FName("Attack_1");
				break;
			case 1:
				Section = FName("Attack_2");
				break;
			default:
				;
			}
			// tako se pokrece montage;
			// prima ptr na UAnimMontage, float za brzinu reprodukcije (stavio duplo brze nego sto je originalna animacija)
			// i jos neke parametre koji imaju default vrednosti
			AnimInstance->Montage_Play(CombatMontage, 2.f);
			// hocemo odredjeni section koji smo kreirali u AnimMontage pod imenom CombatMontage u UE
			AnimInstance->Montage_JumpToSection(Section, CombatMontage);
		}
	}
}

void AMain::AttackEnd()
{
	// zavrsio se napad
	bAttacking = false;
	// nema potrebe za interpolacije Mainove rotacije ako napad nije aktivan
	SetInterpToEnemy(false);

	// kad se zavrsi napad - ako LMB i dalje pritisnut, pocni napad ponovo
	if (bLMBDown)
		Attack();
}

void AMain::PlaySwingSound()
{
	// ako ptr na SoundCue nije nullptr, reprodukuj swing zvuk tokom napada
	if (EquippedWeapon->SwingSound)
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
}

void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

float AMain::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	// skini stetu i vrati float
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die();
		// castuj DamageCauser u Enemyja da bismo mu pristupili korisnim stvarima
		// da bismo mu rekli da odjebe s napadima, dumro je Main vec
		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
				Enemy->bHasValidTarget = false;
		}
	}
	// ako skidas manje nego ostalo healtha, smanji za Amount;
	else
		Health -= DamageAmount;

	return DamageAmount;
}

void AMain::DeathEnd()
{
	// prekini s animacijama
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMain::Jump()
{
	// ako aktivan pause menu, izadji iz f-je
	if (MainPlayerController)
		if (MainPlayerController->bPauseMenuVisible)
			return;

	// parentova f-ja Jump moze da se zove samo ako Main nije mrtav
	if (MovementStatus != EMovementStatus::EMS_Dead)
		ACharacter::Jump();
}

void AMain::UpdateCombatTarget()
{
	// GetOverlappingActors ce u OverlappingActors array strpa sve
	// sto se overlapuje, a da je Enemy ili izvedeno od Enemy
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	// ako nema overlappinga sa enemyjima, ukloni enemyjev HealhBar i napusti objekat
	if (OverlappingActors.Num() == 0)
	{
		if (MainPlayerController)
			MainPlayerController->RemoveEnemyHealthBar();
		return;
	}

	// sad kad smo sigurni da postoji bar jedan, strpaj ga u ClosestEnemy
	// pa cemo posle da azuriramo ako ima neki blizi
	// (mora biti castovan, jer je u arrayu AActor)
	AEnemy *ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);
	// provera da li je uspesan cast, mada malo besmislena, jer smo filtrirali sve sto nije AEnemy
	if (ClosestEnemy)
	{
		// Mainova lokacija
		FVector Location = GetActorLocation();
		// inicijalizujemo MinDistance na razdaljinu do prvog Enemyja;
		// razlika lokacija vraca FVector, pa pozvan .Size() za njegov intenzitet
		float MinDistance = (Location - ClosestEnemy->GetActorLocation()).Size();

		// prodji kroz sve OverlappingActore; svakog castuj u Enemy;
		// ako cast uspesan, racunaj udaljenost Maina od trenutnog enemyja;
		// ako je najmanja do sada, azuriraj MinDistance i ClosesEnemy;
		float DistanceToActor;
		for (auto Actor : OverlappingActors)
		{
			AEnemy *Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
				if (DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
			
		}
		// prikazi enemyjev HealthBar
		if (MainPlayerController)
			MainPlayerController->DisplayEnemyHealthBar();
		// azuriraj Mainov CombatTarget
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
}

void AMain::SwitchLevel(FName LevelName)
{
	/*SaveGame();*/
	// getuj ime trenutne mape; ako nije isto kao prosledjeno ime mape, onda slobodno nastavi
	UWorld *World = GetWorld();
	if (World)
	{
		SaveGame();
		FString CurrentLevel = World->GetMapName();
		/**********/
		CurrentLevel.RemoveFromStart(World -> StreamingLevelsPrefix);

		// da bi promenljiva tipa FName mogla da se poredi sa FString
		FName CurrentLevelName = *CurrentLevel;

		// ako nismo trenutno u levelu koji je prosledjen, predji na taj level
		if (CurrentLevelName != LevelName)
			UGameplayStatics::OpenLevel(World, LevelName);
	}
	//SaveGame();
}

void AMain::SaveGame()
{
	// CreateSaveGameObject kreira instancu SaveGame objekta; prima TSubclassOf<USaveGame>;
	// nacin da se dobije TSubclassOf<USaveGame> je da se pozove nasa kreirana USaveGame koja se zove USameGave01
	// i scope operatorom se pristupi StaticClass(); CreateSaveGameObject vraca USaveGame*;
	// return vrednost f-je castovana u nasu USaveGame01 (koja je podklasa USaveGame) i smestena u promenljivu SaveGameInstance
	USaveGame01 *SaveGameInstance = Cast<USaveGame01>(UGameplayStatics::CreateSaveGameObject(USaveGame01::StaticClass()));

	// sacuvaj sve u strukturu koja je u SaveGameInstance
	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Coins = Coins;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;

	// getuj ime trenutne mape
	FString MapName = GetWorld()->GetMapName();
	// ukloni onaj prefiks koji UE automatski stavi
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	// sacuvaj ime mape bez nnepootrebnog prefiksa
	SaveGameInstance->CharacterStats.LevelName = MapName;

	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	// ako ima oruzje, sacuvaj i njega
	if (EquippedWeapon)
		SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name;
	else
		SaveGameInstance->CharacterStats.WeaponName = TEXT("");

	// sacuva sve podatke iz SaveGame instance u memoriju racunara
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
}

void AMain::LoadGame(bool SetPosition)
{
	// CreateSaveGameObject kreira instancu SaveGame objekta; prima TSubclassOf<USaveGame>;
	// nacin da se dobije TSubclassOf<USaveGame> je da se pozove nasa kreirana USaveGame koja se zove USameGave01
	// i scope operatorom se pristupi StaticClass(); CreateSaveGameObject vraca USaveGame*;
	// return vrednost f-je castovana u nasu USaveGame01 (koja je podklasa USaveGame) i smestena u promenljivu LoadGameInstance
	USaveGame01 *LoadGameInstance = Cast<USaveGame01>(UGameplayStatics::CreateSaveGameObject(USaveGame01::StaticClass()));

	// loaduje sve podatke iz slota racunara; LoadGameFromSlot vraca USaveGame*, pa je zato
	// castujemo u USaveGame01;
	LoadGameInstance = Cast<USaveGame01>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	// punimo Mainove podatke onakvim kakve smo loadovali
	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Coins = LoadGameInstance->CharacterStats.Coins;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;

	// ako WeaponStorage nije nullptr (a ne bi trebalo da je, odabrali smo u Main_BP da je ItemStorage_BP),
	// spawnnuj actora tipa AItemStorage, konkretno spawnuj WeaponStorage i to smesti u promenljivu Weapons
	if (WeaponStorage)
	{
		AItemStorage *Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			// procitaj ime weapona koje je sacuvano i kreiraj nov weapon za equipovanje
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;
			if (WeaponName != TEXT(""))
			{
				// spawnovace bas onaj weapon koji je iscitao iz mape, a koji ima key koji odgovara
				// prosledjenom stringu
				AWeapon *WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
				// opremi ovog junaka
				WeaponToEquip->Equip(this);
			}
		}
	}

	// lokaciju i rotaciju ne zelimo da setujemo kad predje iz jednog levela u drugi;
	// jer lokacija u jednom levelu nema veze s lokacijom u drugom i spawnovace ga ko zna gde;
	// isto vazi i za rotaciju
	if (SetPosition)
	{
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	}

	// kad se loaduje igra, postavi ga u normalan movement status
	SetMovementStatus(EMovementStatus::EMS_Normal);

	// aktiviraj animacije ponovo
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;

	// ako sacuvani level nije prazan string, onda mozes nastaviti
	if (LoadGameInstance->CharacterStats.LevelName != TEXT(""))
	{
		// getuj sacuvali level
		FName LevelName = *LoadGameInstance->CharacterStats.LevelName;
		// unutar SwitchLevel vec postoji provera da li je prosledjeni level isti kao trenutni level;
		// ako nije, switchuj
		SwitchLevel(LevelName);
	}

	//// postavi game mode na GameModeOnly da ne bi mis i dalje kliktao van igre
	//if (MainPlayerController)
	//	MainPlayerController->GameModeOnly();
}

void AMain::ESCDown()
{
	bESCDown = true;

	// na pritisak ESC, prikazi/sakrij PauseMenu
	if (MainPlayerController)
		MainPlayerController->TogglePauseMenu();
}

void AMain::ESCUp()
{
	bESCDown = false;
}

bool AMain::CanMove(float Value)
{
	// Controller se nasledjuje iz Pawn;
	// iz documentation: controllers are non-physical actors that can possess a pawn to control its actions;
	// a player controller is used by human players to control pawns;
	// ne treba nam provera da li je controller nullptr jer vec provaravao MainPlayerController;
	// ako MainPlayerController nije nullptr i ako pritisnuto dugme (u svakom frejmu se dobija Value - ako pritisnuto, -1 ili 1; ako nije, 0)
	// i ako nije u rezimu napada (necemo da karakter moze da se krece dok napada) i ako nije mrtav i ako nije aktivan pause menu

	if (MainPlayerController)
		return (Value != 0.f) && (!bAttacking) &&
		(MovementStatus != EMovementStatus::EMS_Dead) &&
		(!MainPlayerController->bPauseMenuVisible);

	return false;
}

void AMain::Turn(float Value)
{
	// AddControllerUGAOInput je f-ja pristuna u svim APawn
	if (CanMove(Value))
		AddControllerYawInput(Value);
}

void AMain::LookUp(float Value)
{
	// AddControllerUGAOInput je f-ja pristuna u svim APawn
	if (CanMove(Value))
		AddControllerPitchInput(Value);
}

void AMain::LoadGameNoSwitchLevel()
{
	// CreateSaveGameObject kreira instancu SaveGame objekta; prima TSubclassOf<USaveGame>;
	// nacin da se dobije TSubclassOf<USaveGame> je da se pozove nasa kreirana USaveGame koja se zove USameGave01
	// i scope operatorom se pristupi StaticClass(); CreateSaveGameObject vraca USaveGame*;
	// return vrednost f-je castovana u nasu USaveGame01 (koja je podklasa USaveGame) i smestena u promenljivu LoadGameInstance
	USaveGame01 *LoadGameInstance = Cast<USaveGame01>(UGameplayStatics::CreateSaveGameObject(USaveGame01::StaticClass()));

	// loaduje sve podatke iz slota racunara; LoadGameFromSlot vraca USaveGame*, pa je zato
	// castujemo u USaveGame01;
	LoadGameInstance = Cast<USaveGame01>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	// punimo Mainove podatke onakvim kakve smo loadovali
	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Coins = LoadGameInstance->CharacterStats.Coins;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;

	// ako WeaponStorage nije nullptr (a ne bi trebalo da je, odabrali smo u Main_BP da je ItemStorage_BP),
	// spawnnuj actora tipa AItemStorage, konkretno spawnuj WeaponStorage i to smesti u promenljivu Weapons
	if (WeaponStorage)
	{
		AItemStorage *Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			// procitaj ime weapona koje je sacuvano i kreiraj nov weapon za equipovanje
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;
			if (WeaponName != TEXT(""))
			{
				// spawnovace bas onaj weapon koji je iscitao iz mape, a koji ima key koji odgovara
				// prosledjenom stringu
				AWeapon *WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
				// opremi ovog junaka
				WeaponToEquip->Equip(this);
			}
		}
	}

	// kad se loaduje igra, postavi ga u normalan movement status
	SetMovementStatus(EMovementStatus::EMS_Normal);

	// aktiviraj animacije ponovo
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;

	//// postavi game mode na GameModeOnly da ne bi mis i dalje kliktao van igre
	//if (MainPlayerController)
	//	MainPlayerController->GameModeOnly();
}