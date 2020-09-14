#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// kreiraj mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	// postavi mesh da bude root component;
	RootComponent = Mesh;

	// inicijalizacija start i end point;
	StartPoint = FVector(0.f);
	EndPoint = FVector(0.f);
	
	// inicijalizacija brzine interpolacije;
	InterpSpeed = 0.5f;

	// inicijalno po default ne treba interpolacija;
	bInterping = false;

	// inicijalizacija trajanja pauze izmedju interpolacija;
	InterpTime = 3.f;

}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();

	// postavi platformu na lokaciju na koju je prevucemo;
	StartPoint = GetActorLocation();
	// posto je EndPoint dat relativno u odnosu na StartPoint, dodajemo lokaciju StartPointa;
	EndPoint += StartPoint;

	// pokreni tajmer; kad istekne vreme InterpTime, pozovi ToogleInterping;
	GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);

	// distanca izmedju start i end tacke;
	Distance = (EndPoint - StartPoint).Size();
	
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ako treba interpolirati
	if (bInterping)
	{
		// trenutna lokacija u svakom frejmu; bice potrebna za interpolaciju lokacije platforme;
		FVector CurrentLocation = GetActorLocation();
		// vrati neku interpoliranu vrednost izmedju trenutne i ciljane lokacije, brzinom InterpSpeed;
		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);
		// azuriraj lokaciju;
		SetActorLocation(Interp);

		// koliki put platforma prevalila;
		float DistanceTraveled = (GetActorLocation() - StartPoint).Size();

		// ako je dosla do odredista
		if (Distance - DistanceTraveled <= 1.f)
		{
			// switchuj boolean, sad treba da pauzira;
			ToggleInterping();
			// setuj tajmer na InterpTime; kad istekne vreme, zovi f-ju da promeni bool da treba da interpolira;
			GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);
			// zameni vrednosti start i end vektora, da bi interpolirao nazad na start poziciju i tako stalno;
			SwapVectors(StartPoint, EndPoint);
		}
	}
}

void AFloatingPlatform::ToggleInterping()
{
	bInterping = !bInterping;
}

void AFloatingPlatform::SwapVectors(FVector& V1, FVector& V2)
{
	FVector Tmp = V1;
	V1 = V2;
	V2 = Tmp;
}