#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Enemy.h"
#include "AIController.h"


// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// kreiranje spawning boxa;
	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	// dodaj actore za spawnovanje u array
	if (Actor_1 && Actor_2 && Actor_3 && Actor_4)
	{
		SpawnArray.Add(Actor_1);
		SpawnArray.Add(Actor_2);
		SpawnArray.Add(Actor_3);
		SpawnArray.Add(Actor_4);
	}
	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetSpawnPoint()
{
	// vraca velicinu boxa;
	FVector Extent = SpawningBox->GetScaledBoxExtent();
	// vraca origin location komponente;
	FVector Origin = SpawningBox->GetComponentLocation();

	// vraca random tacku unutar zatvorenog prostora koji cini Spawning Box;
	return UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
}

void ASpawnVolume::SpawnOurActor_Implementation(UClass* ToSpawn, FVector const& Location)
{
	// proverava da nije nullptr;
	if (ToSpawn)
	{
		// kreiraj world
		UWorld* World = GetWorld();

		// ako World validan
		if (World)
		{
			// f-ja SpawnActor vraca Actora kojeg spawnuje;
			// f-ja SpawnActor prima:
			// UClass*, to je ovaj ToSpawn sto smo prosledili;
			// Location koji prosledjujemo;
			// Rotation koji postavljamo na 0 u svim osama;
			// neki FActorSpawnParameters, pa je zbog toga kreirana varijabla SpawnParams;
			FActorSpawnParameters SpawnParams;
			AActor *Actor = World->SpawnActor<AActor>(ToSpawn, Location, FRotator(0.f), SpawnParams);

			// ako je spawnovao bas enemyja (a ne bombu ili sta god)
			AEnemy *Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				// spawnuj i setuj enemyju AIControllera
				Enemy->SpawnDefaultController();

				// ako mu je controller bas AAIController, onda Enemyju setuj promenljivu
				// AIController na ovog controllera kojeg je dobio iz SpawnDefaultController()
				AAIController *AICont = Cast<AAIController>(Enemy->GetController());
				if (AICont)
					Enemy->AIController = AICont;
			}
		}
	}
}

TSubclassOf<AActor> ASpawnVolume::GetSpawnActor()
{
	// radi samo ako niz actora za spawnovanje nije 0
	if (SpawnArray.Num() > 0)
	{
		int32 Selection = FMath::RandRange(0, SpawnArray.Num() - 1);

		// vrati random izabranog actora iz arraya
		return SpawnArray[Selection];
	}
	else
		return nullptr;
}