// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "Enemy.h"
#include "Components/CapsuleComponent.h"

AExplosive::AExplosive()
{
	// default vrednost za stetu;
	Damage = 10.f;
}

void AExplosive::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// pozovi roditeljsku f-ju, pa onda loguj;
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	// ako actor s kojim dolazi do overlapovanja nije nullptr;
	if (OtherActor)
	{
		// castuj OtherActora u Main; ako je vec Main, cast je uspesan; ako ne, Main ce biti nullptr;
		AMain *Main = Cast<AMain>(OtherActor);
		// castuj OtherActora u Enemy, jer eksplozivi mogu da uticu i na enemyja
		AEnemy *Enemy = Cast<AEnemy>(OtherActor);

		// ako cast bio uspesan, smanji health
		if (Main || Enemy)
		{
			// samo ako se overlapuje sa capsule komponentom (ne zelimo efekat da se
			// desava kad se overlapuje sa enemyjevom agro sferom npr.)
			UCapsuleComponent *CapsuleComponent = Cast<UCapsuleComponent>(OtherComp);
			if (CapsuleComponent)
			{
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

				// ostecen je OtherActor, bio on Main ili Enemy; steta koja je naneta = Damage;
				// za AController* moze da se prosledi nullptr; DamageCauser je this; DamageTypeClass
				UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, DamageTypeClass);
				// kad se desi overlap, samounisti actora (bombu);
				Destroy();
			}
		}
	}

}

void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// pozovi roditeljsku f-ju, pa onda loguj;
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	UE_LOG(LogTemp, Warning, TEXT("Explosive::OnOverlapEnd"));
}