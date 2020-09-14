#include "LevelTransitionVolume.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "Main.h"

// Sets default values
ALevelTransitionVolume::ALevelTransitionVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// kreiraj transition volume i setuj ga da bude root component
	TransitionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TransitionVolume"));
	RootComponent = TransitionVolume;

	// kreiramo billboard (ona glava koja stoji unutar boxa, da mozemo lakse da selektujemo
	// box); moze i u UE da se ubaci, ubacili smo je za SpawnVolume
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(GetRootComponent());

	// default vrednost za ime levela
	TransitionLevelName = "SunTemple";

}

// Called when the game starts or when spawned
void ALevelTransitionVolume::BeginPlay()
{
	Super::BeginPlay();

	// bind overlapping event sa f-jom koju programiramo za overlap
	TransitionVolume->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransitionVolume::OnOverlapBegin);
	
}

// Called every frame
void ALevelTransitionVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelTransitionVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// ako se bas Main overlapuje sa boxom, pozovi njegovu f-ju za prelazak u drugi level
	if (OtherActor)
	{
		AMain *Main = Cast<AMain>(OtherActor);
		if (Main)
			Main->SwitchLevel(TransitionLevelName);
	}
}