#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Main.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayerController.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// kreiranje AgroSphere, setovanje root komponente, postavljanje radijusa
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(800.f);

	// kreiranje CombatSphere, setovanje root komponente, postavljanje radijusa
	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);
	
	// kreiranje boxa koji oznacava deo pauka koji pravi stetu
	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	//// AttachToComponent attachuje box komponentu na neku drugu;
	//// prima USceneComponent*, FAttachmentTransformRules, FName
	//CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("EnemySocket"));
	// u logu se pojavljivala (nefatalna) greska, koja predlaze da se koristi SetupAttachment
	CombatCollision->SetupAttachment(GetMesh(), FName("EnemySocket"));

	// presets za CombatCollision box komponentu
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// default vrednost indikatora za preklapanje sa combat sferom
	bOverlappingCombatSphere = false;

	// default vrednosti za health i damage
	Health = 100.f;
	MaxHealth = 100.f;
	Damage = 5.f;

	// default parametri za interpolaciju Enemyja ka Mainu
	InterpSpeed = 15.f;
	bInterpToMain = false;

	// min i max cekanje da enemy zamahne
	AttackTimeMin = .3f;
	AttackTimeMax = 1.75f;

	// default pozicija u Idle
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);

	// koliko da ceka pre nego sto enemy nestane sa scene kad umre
	DeathDelay = 3.f;

	// indikator da li ima CombatTarget
	bHasValidTarget = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// castujemo Controllera u AIControllera, pa ako uspesan cast, znaci da je Controller upravo AIController
	AIController = Cast<AAIController>(GetController());

	// bindovanje samih dogadjaja overlapovanja sa odgovarajucim nasim f-jama
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);
	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	// bindovanje overlapovanja boxa na paukovoj nozi za napad, sa f-jama za overlap
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	// ne zelimo da nam se kamera umece izmedju enemyja i Maina svaki put kad se enemy nadje izmedju Maina i kamere
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ako je indikator za interpoliranje true i CombatTarget nije nullptr
	if (bInterpToMain && CombatTarget)
	{
		// nadji ciljanu rotaciju
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		// interpoliraj rotaciju; prima trenutnu, ciljanu, DeltaTime i brzinu interpolacije koju smo vec definisali;
		// vraca rotaciju za trenutni frejm, omogucena smooth tranzicija
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

}

FRotator AEnemy::GetLookAtRotationYaw(FVector Target)
{
	// vraca ciljanu rotaciju prilikom interpolacije; zanima nas samo Yaw;
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	return FRotator(0.f, LookAtRotation.Yaw, 0.f);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// ako OtherActor nije nullptr i enemy ziv, castuj OtherActora u Main
	if (OtherActor && Alive())
	{
		// ako jeste Main taj koji se sudara sa agro sferom, zovi MoveToTarget
		AMain *Main = Cast<AMain>(OtherActor);
		if (Main)
			MoveToTarget(Main);
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// ako je OtherActor bas main karakter, vrati enemyja u Idle, jer je zavrsio overlap sa agro sferom
	if (OtherActor)
	{
		AMain *Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			// nema koga da juri sad, izasao iz agro sfere
			bHasValidTarget = false;

			// ako je bas ova instanca Mainov neprijatelj
			if (Main->CombatTarget == this)
			{
				// indikator da Enemyjev HealthBar ne treba vise da se prikazuje
				Main->SetHasCombatTarget(false);
				// setuj CombatTarget Mainu na nullptr
				Main->SetCombatTarget(nullptr);
			}
			// kad prestane overlap sa agro sferom, necemo vise da se prikazuje Enemyjev HealthBar
			Main->UpdateCombatTarget();

			// vrati se u Idle stanje, ali i prinudno zaustavi kretanje momentalno
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			if (AIController)
				AIController->StopMovement();
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// ako je OtherActor bas main karakter i enemy je ziv, postavi Mainov CombatTarget na this enemyja,
	// CombatTarget na Maina, regulisi indikator za overlapovanje combat sfere i pozovi f-ju Attack()
	if (OtherActor && Alive())
	{
		AMain *Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			// ima s kim da se siba
			bHasValidTarget = true;

			Main->SetCombatTarget(this);

			// Main ima CombatTarget, azuriraj mu indikator
			Main->SetHasCombatTarget(true);

			// azuriraj CombatTarget koji unutar sebe postavi i display enemyjevog HealthBara
			Main->UpdateCombatTarget();

			CombatTarget = Main;
			bOverlappingCombatSphere = true;
			// sibaj Maina samo ako nije mrtav (uz pauzicu)
			if (!(Main->bDead))
			{
				// random vreme koje da ceka izmedju dva enemyjeva zamaha
				float AttackTime = FMath::RandRange(AttackTimeMin, AttackTimeMax);
				GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// ako je OtherActor bas main karakter, ponovo pokreni MoveToTarget,
	// jer je zavrsio overlap sa combat sferama;
	// zatim postavi na nullptr CombatTarget, vise nam ne treba;
	if (OtherActor)
	{
		AMain *Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			bOverlappingCombatSphere = false;
			MoveToTarget(Main);
			CombatTarget = nullptr;

			// ako se overlap sa combat sferom zavrsi, a Mainov CombatTarget je neki
			// drugi enemy, onda ne treba da mu menjamo CombatTarget
			if (Main->CombatTarget == this)
			{
				// azuriraj Mainov CombatTarget
				Main->SetCombatTarget(nullptr);
				Main->bHasCombatTarget = false;
				Main->UpdateCombatTarget();
			}

			// ako se overlap zavrsi, skloni HealthBar, ali samo ako je komponenta s kojom
			// se overlap zavrsio Mainova mesh komponenta
			if (Main->MainPlayerController)
			{
				USkeletalMeshComponent *MainMesh = Cast<USkeletalMeshComponent>(OtherComp);
				if (MainMesh)
					Main->MainPlayerController->RemoveEnemyHealthBar();
			}

			// ako prestao overlap sa combat sferom, resetuj tajmer za delay napada
			GetWorldTimerManager().ClearTimer(AttackTimer);
		}
	}
}

void AEnemy::MoveToTarget(AMain *Target)
{
	// azuriraj MovementStatus na MoveToTarget
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	// ako AIController nije nullptr, krairaj parametre potrebne za koriscenje MoveTo f-je
	if (AIController)
	{
		// prvi parametar koji ide u MoveTo
		FAIMoveRequest MoveRequest;
		// razdaljina posle koje ce se smatrati da je dosao do MoveTo cilja
		MoveRequest.SetAcceptanceRadius(10.f);
		// sta mu je cilj, do cega da ide
		MoveRequest.SetGoalActor(Target);

		// drugi parametar koji ide u MoveTo; u njega smesten rezultat MoveTo f-je
		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);

		//// array path pointsa koji su smesteni u NavPath
		//TArray<FNavPathPoint> PathPoints = NavPath->GetPathPoints();
		//// iteriramo kroz svaki path point i crtamo debug sferu
		//for (auto Point : PathPoints)
		//{
		//	FVector Location = Point.Location;
		//	// nacrtaj zelenu sferu oko pointa na lokaciji pointa,
		//	// radijusa 25, 8 segmenata, crvene boje, 10 sekundi trajanja i thicknessa 2.5;
		//	// uglavnom pratio sugerisanje intelisensa za parametre;
		//	DrawDebugSphere(GetWorld(), Location, 25.f, 8, FColor::Red, false, 10.f, (uint8)'\000', 2.5f);
		//}
	}
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// ako OtherActor nije nullptr, castuj ga u Maina
	if (OtherActor)
	{
		// ako je OtherActor bas Main i njegov HitParticles nije nullptr, spawnuj efekte na lokaciji TipSocket-a na dnu paukove noge
		AMain *Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (Main->HitParticles)
			{
				// getuj TipSocket koji smo kreirali na paukovom skeletonu
				const USkeletalMeshSocket *TipSocket = GetMesh()->GetSocketByName("TipSocket");
				// ako nije nullptr, getuj njegovu lokaciju i spawnuj particle effect na toj lokaciji
				if (TipSocket)
				{
					FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}

			// ako su Main i njegov ptr na SoundCue validan, pusti zvuk kad se desio overlap sa
			// boxom na paukovoj nozi
			if (Main->HitSound)
				UGameplayStatics::PlaySound2D(this, Main->HitSound);

			// apply damage od strane ovog enemyja na maina
			if (DamageTypeClass)
				UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// ako nije nullptr, pusti zvuk dok pauk zamahuje nogomm tokom napada
	if (SwingSound)
		UGameplayStatics::PlaySound2D(this, SwingSound);
}

void AEnemy::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
	// jesi ziv i je l' imas na koga da kidises?
	if (Alive() && bHasValidTarget)
	{
		// prestaje s kretanjem i krece samo napad
		if (AIController)
		{
			AIController->StopMovement();
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		}
		// ako nije vec u rezimu napada
		if (!bAttacking)
		{
			bAttacking = true;
			// setuj interpolaciju ka Mainu kad krene napad na true
			SetInterpToMain(true);

			UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
			// ako nije nullptr, pusti montazu brzinom 1.5 i skoci na section koji smo nazvali Attack
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(CombatMontage, 1.5f);
				AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
			}
		}
	}
}

void AEnemy::AttackEnd()
{
	// zavrsio se napad
	bAttacking = false;
	// nema potrebe za interpolacije Enemyjeve rotacije ako napad nije aktivan
	SetInterpToMain(false);
	// ako se i dalje combat sfera preklapa s igracem, pozovi Attack ponovo kad istekne neko random vreme
	if (bOverlappingCombatSphere)
	{
		// random vreme koje da ceka izmedju dva enemyjeva zamaha
		float AttackTime = FMath::RandRange(AttackTimeMin, AttackTimeMax);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
	}
}

void AEnemy::SetInterpToMain(bool Interp)
{
	bInterpToMain = Interp;
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	// ako health dosao kraja, dumri
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die(DamageCauser);
	}
	else
		Health -= DamageAmount;

	// mora da vrati neki float
	return DamageAmount;
}

void AEnemy::Die(AActor *Causer)
{
	UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
	// ako nisu nullptr, pusti montazu normalnom brzinom i skoci na section koji smo nazvali Death
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.f);
		AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
	}
	// setuj enum stanje na Dead i sve sfere za koliziju deaktiviraj
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);

	SetInterpToMain(false);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ako je Main ubio enemyja, pozovi UpdateCombatTarget
	AMain *Main = Cast<AMain>(Causer);
	if (Main)
		Main->UpdateCombatTarget();
}

void AEnemy::DeathEnd()
{
	// kad umre, nema potrebe za animacijama vise
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	// pokreni tajmer za nestajanje sa scene i onda pozovi f-ju koja brise actora
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

bool AEnemy::Alive()
{
	return (GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead);
}

void AEnemy::Disappear()
{
	// ugradjena f-ja za unistavanje this actora
	this->Destroy();
}