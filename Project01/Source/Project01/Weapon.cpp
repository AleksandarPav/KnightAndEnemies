#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"


AWeapon::AWeapon()
{
	// kreiraj mesh i postavi mu root component, koja je u Itemu definisana kao CollisionVolume
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	// kreiraj volume oko ostrice za sudare s enemyjem i postavi mu root component
	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	// presets za CombatCollision box komponentu
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// po default, mac nema particle effect
	bWeaponParticleEffect = false;

	// po default, Weapon je u Pickup stanju, a ne Equipped
	WeaponState = EWeaponState::EWS_Pickup;

	// default steta koju pravi oruzje
	Damage = 10.f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	// bindovanje eventa overlapovanja sa nasim f-jama
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	// kad pocne overlap, ako nema nullptra i ako je oruzje u stanju Pickup, postavi ovo oruzje kao aktivan item u OtherActoru
	// s kojim se oruzje overlapuje, ako je taj OtherActor bas Main;
	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor)
	{
		AMain *Main = Cast<AMain>(OtherActor);
		if (Main)
			Main->SetActiveOverlappingItem(this);
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	
	// kad prestane overlap, ako nema nullptra, ukloni ovo oruzje iz aktivnog itema u OtherActoru
	// s kojim se oruzje overlapuje, ako je taj OtherActor bas Main; postavi aktivan item na nullptr;
	// na taj nacin, kad overlap prestane, aktivan item nije zaostao u Mainu;
	if (OtherActor)
	{
		AMain *Main = Cast<AMain>(OtherActor);
		if (Main)
			Main->SetActiveOverlappingItem(nullptr);
	}
}

void AWeapon::Equip(AMain *Character)
{
	// ako nije nullptr
	if (Character)
	{
		// postavi instigatora
		SetInstigator(Character->GetController());

		// ako vec ima mac, unisti taj koji poseduje
		if (Character->EquippedWeapon)
			Character->EquippedWeapon->Destroy();


		// da kamera ignorise weapon kad se nadje izmedju kamere i karaktera, da ne bi zumirala karaktera u tim situacijama
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		// takodje ne zelimo nikakav collision sa samim pawnom koji nosi weapon
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		// hocemo da ignorise fiziku, jer je weapon attachhovan za karaktera, ne treba nam tipa gravitacija
		SkeletalMesh->SetSimulatePhysics(false);

		// u Character - Mesh - SkeletalMesh prikacili socket za desnu saku i nazvali ga RightHandSocket;
		// taj kreirani socket sad getujemo by name, a varijabla RightHandSocket ce sadrzati ptr na njega;
		const USkeletalMeshSocket *RightHandSocket = Character->GetMesh()->GetSocketByName("RightHandSocket");
		
		// ako nije nullptr attachuj ovog actora (weapon je podklasa Actora), za karakterov mesh
		if (RightHandSocket)
		{
			RightHandSocket->AttachActor(this, Character->GetMesh());
			// nemoj da se rotira mac dok ga ovaj drzi
			bRotate = false;
			// setuj oruzje igraca na ovo oruzje
			Character->SetEquippedWeapon(this);
			// kad se overlapuju i igrac uzme weapon, nema potrebe da taj weapon vise bude ActiveOverlappingItem
			Character->SetActiveOverlappingItem(nullptr);
		}
		// ako OnEquipSound nije nullptr, pusti zvuk
		if (OnEquipSound)
			UGameplayStatics::PlaySound2D(this, OnEquipSound);

		// ako necemo da weapon ima particle effect, iskljuci particle effects
		if (!bWeaponParticleEffect)
			IdleParticlesComponent->Deactivate();
		
	}
}

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// ako OtherActor nije nullptr, castuj ga u Enemyja
	if (OtherActor)
	{
		// ako je OtherActor bas Enemy i njegov HitParticles nije nullptr, spawnuj efekte na lokaciji WeaponSocket-a
		AEnemy *Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (Enemy->HitParticles)
			{
				// getuj weapon socket koji smo kreirali na skeletal meshu od maca
				const USkeletalMeshSocket *WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
				// ako nije nullptr, getuj njegovu lokaciju i spawnuj particle effect na toj lokaciji
				if (WeaponSocket)
				{
					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			// ako je enemy i njegov ptr na SoundCue validan, pusti zvuk kad se desio overlap sa
			// combat sferom oruzja
			if (Enemy->HitSound)
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			// apply damage i osteti enemyja
			if (DamageTypeClass)
				UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
		}
		
	}
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWeapon::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}