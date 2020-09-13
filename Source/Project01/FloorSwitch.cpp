// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwitch.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AFloorSwitch::AFloorSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	// kad se pritisne overlapuje trigger box, treba dugme da se spusti u pod i treba vrata da se dignu - dve akcije koje su vezane
	// za trigger box; zato ga stavljamo da bude root component;
	RootComponent = TriggerBox;

	// omoguci sudare, ali bez fizike, samo overlaping;
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// definisi koji je tip objekta za sudare;
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	// po default, necemo nikakvu akciju kad se desi kolizija sa bilo cim;
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// ali u specijalnom slucaju, hocemo da kad se sudara sa main igracem, koji je pawn, ima reakciju, i to Overlap;
	// ECC - EnumCollisionChanel, ECR - EnumCollisionResponse;
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// hard code za sad shape trigger boxa;
	TriggerBox->SetBoxExtent(FVector(62.f, 62.f, 32.f));

	// kreiranje floor swtich mesha koji je vezan za TriggerBox; ne mora GetRootComponent(), moze i samo RootComponent;
	FloorSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorSwitch"));
	FloorSwitch->SetupAttachment(GetRootComponent());

	// kreiranje Door mesha koji je vezan za TriggerBox; ne mora GetRootComponent(), moze i samo RootComponent;
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(GetRootComponent());

	// koliko timer da ceka kad igrac sidje s trigger boxa pre nego sto krene switch da se podize i vrata da se spustaju;
	SwitchTime = 2.f;

	bCharacterOnSwitch = false;

}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();
	
	// koje funkcije da se pozovu kad se desi odredjeni event;
	// bind a function to an overlap event;
	// AddDynamic je makro koji prima objekat i ptr na funkciju koja treba da se desi;
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapEnd);

	// postavi inicijalne lokacije na one koje su trenutno u levelu;
	InitialDoorLocation = Door->GetComponentLocation();
	InitialSwitchLocation = FloorSwitch->GetComponentLocation();
}

// Called every frame
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorSwitch::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Begin!"));

	// ako vec ne stoji na switchu, sad stoji;
	if (!bCharacterOnSwitch)
		bCharacterOnSwitch = true;

	// digni vrata;
	RaiseDoor();
	// spusti switch;
	LowerFloorSwitch();
}

void AFloorSwitch::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap End!"));

	if (bCharacterOnSwitch)
		bCharacterOnSwitch = false;

	// pokreni timer;
	GetWorldTimerManager().SetTimer(SwitchHandle, this, &AFloorSwitch::CloseDoor, SwitchTime);
}

void AFloorSwitch::UpdateDoorLocation(float Z)
{
	// novoj lokaciji povecaj samo Z komponentu;
	FVector NewLocation = InitialDoorLocation;
	NewLocation.Z += Z;
	// postavi vratima novu lokaciju;
	Door->SetWorldLocation(NewLocation);
}

void AFloorSwitch::UpdateFloorSwitchLocation(float Z)
{
	// novoj lokaciji povecaj (odnosno smanji, jer kriva u Timeline u BP ide na dole) samo Z komponentu;
	FVector NewLocation = InitialSwitchLocation;
	NewLocation.Z += Z;
	// postavi switchu novu lokaciju;
	FloorSwitch->SetWorldLocation(NewLocation);
}

void AFloorSwitch::CloseDoor()
{
	// radi spustanje vrata i podizanje switcha samo ako igrac ne stoji na switchu
	if (!bCharacterOnSwitch)
	{
		// spusti vrata;
		LowerDoor();
		// digni switch;
		RaiseFloorSwitch();
	}
}