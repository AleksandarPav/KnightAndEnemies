#include "Pickup.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"

APickup::APickup()
{
}

void APickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// pozovi roditeljsku f-ju, pa onda loguj;
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	// ako actor s kojim dolazi do overlapovanja nije nullptr;
	if (OtherActor)
	{
		// castuj OtherActora u Main; ako je vec Main, cast je uspesan; ako ne, Main ce biti nullptr;
		AMain *Main = Cast<AMain>(OtherActor);
		// ako cast bio uspesan, uvecaj novce
		if (Main)
		{
			// f-ja implementirana u BPs; ponasanje u skladu s overlapovanjem
			OnPickupBP(Main);

			// ako nije nullptr
			if (OverlapParticles)
				// ovaj overload prima world, particle system (zato kreiran ParticleSystem, a ne ParticleSystemComponent),
				// FVector lokacije, FRotator rotacije i neki bool za auto destroy;
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);

			// ako nije nullptr
			if (OverlapSound)
				// pusti zvuk kad se overlapuju igrac i item;
				// prima world context object, USoundBase* (SoundCue nasledjuje SoundBase) i jos neke parametre koji imaju default vrednosti;
				UGameplayStatics::PlaySound2D(this, OverlapSound);

			// dodaj lokaciju lovcica u vektor za lokacije
			Main->PickupLocations.Add(GetActorLocation());
			// kad se desi overlap, samounisti actora (paricu);
			Destroy();
		}
	}
}

void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// pozovi roditeljsku f-ju, pa onda loguj;
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
