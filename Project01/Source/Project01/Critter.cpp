#include "Critter.h"
// za staticmesh
//#include "Components/StaticMeshComponent.h"
// za skeletal mesh
#include "Components/SkeletalMeshComponent.h"
// za camera
#include "Camera/CameraComponent.h"
// za Input u SetupInputComponent
#include "Components/InputComponent.h"

// Sets default values
ACritter::ACritter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// root component se nasledjuje od actora;
	// kreira scene component i inicijalizuje je;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	// inicjalizuj mesh component
	// MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	// attachuj mesh component to the root component;
	MeshComponent->SetupAttachment(GetRootComponent());

	// kreiraj kameru koja je attachovana za actora;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// attach camera to the root component;
	Camera->SetupAttachment(GetRootComponent());
	// setuj poziciju kamere (odakle ce da gleda u junaka actora i pod kojim uglom);
	Camera->SetRelativeLocation(FVector(-300.f, 0.f, 300.f));
	Camera->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));


	/* kad krene Play, hocemo da se sve vidi iz perspektive kamere koja prati actora;
	dole levo u C++ Classes -> Project01 dat default GameModeBase;
	od njega se napravi Blueprint (CritterGameMode_BP) i u BPEditoru sa strane desno ima
	Default Pawn Class - tu se odabere Critter_BP (BP koji je napravljen od Critter klase);
	onda se ovde dodeli kog playera da prati kamera (Player0 je prvi u multiplayer i jedini u single player); */
	// AutoPossessPlayer = EAutoReceiveInput::Player0; // zakomentarisana komanda jer necemo da possess-ujemo pauka;
	// i u Critter_BP (self) treba staviti Auto Possess Player na Disabled;
	/* UE->Settings->World Settings;
	sa desne strane Game Mode Override -> selektovati CritterGameMode_BP (tj. onaj BP koji smo napravili);
	vise nije default da se krece sa WASD, mora rucno da pravi user input;
	mozda treba i da se ode u Critter_BP, klikne na Critter_BP(self) i sa desne strane ima polje
	Auto Possess Player - selektovati na Player0; */

	// inicijalizujemo trneutnu brzinu upravljanja pawnom na 0;
	CurrentVelocity = FVector(0.f);
	// incijalizujemo faktor brzine na 100;
	MaxSpeed = 100.f;
}

// Called when the game starts or when spawned
void ACritter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACritter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// novu lokaciju izracunaj tako sto na trenutnu lokaciju dodas predjeni put (brzina * vreme);
	FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
	SetActorLocation(NewLocation);

}

// Called to bind functionality to input
void ACritter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// BindAxis prima ime axis-a koje smo dali u UE->Edit->Project Settings,
	// pointer na objekat koji ce to sve da handleuje,
	// pointer na odgovarajucu f-ju; f-ja definisana dole, deklarisana u .h fajlu;
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACritter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACritter::MoveRight);
	// ovo bind-uje pravac kretanja, odnosno pritisak odgovarajuceg dugmeta sa metodama MoveForward i MoveRight;
	// kad se pritisne odgovarajuce dugme, ono ce pozvati MoveForward ili MoveRight metodu i prosledice im axis vrednost;
	// axis vrednost ce biti 0 ili 1 (ili -1 ako je tako mapirano u Project Settings);
}

// MoveForward i MoveRight ce da kontrolisu brzinu kontrole kretanja, a lokacija se menja u Tick metodi;
void ACritter::MoveForward(float Value)
{
	// daj mu trenutnu brzinu u X pravcu kojom ce da ide kontrola kretanja;
	// kad stigne -1, ide unazad;
	// Clamp sluzi da osigura da ce input koji dolazi kroz Value da bude in range [-1, 1], pa se to skalira brzinom MaxSpeed;
	CurrentVelocity.X = FMath::Clamp(Value, -1.f, 1.f) * MaxSpeed;
}
void ACritter::MoveRight(float Value)
{
	// daj mu trenutnu brzinu u Y pravcu kojom ce da ide kontrola kretanja;
	// kad stigne -1, ide levo;
	// Clamp sluzi da osigura da ce input koji dolazi kroz Value da bude in range [-1, 1], pa se to skalira brzinom MaxSpeed;
	CurrentVelocity.Y = FMath::Clamp(Value, -1.f, 1.f) * MaxSpeed;
}
