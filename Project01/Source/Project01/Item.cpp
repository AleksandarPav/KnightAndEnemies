#include "Item.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Main.h"


// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// prostor za kolizije sa igracem;
	CollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionVolume"));
	// postavi taj prostor za root component;
	RootComponent = CollisionVolume;
	
	// kreiraj mesh komponentu;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	// kreiraj particle komponentu;
	IdleParticlesComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IdleParticlesComponent"));
	IdleParticlesComponent->SetupAttachment(GetRootComponent());

	// po default, item ne treba da se rotira dok je u idle;
	bRotate = false;
	// default vrednost za brzinu rotacije;
	RotationRate = 45.f;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	// kad krene/zavrsi se kolizija, zovi f-je OnOverlapBegin/End;
	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AItem::OnOverlapEnd);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ako item treba da rotira
	if (bRotate)
	{
		// uzmi trenutnu rotaciju
		FRotator Rotation = GetActorRotation();
		// povecaj joj Yaw ugao za rotation rate times delta time
		Rotation.Yaw += DeltaTime * RotationRate;
		// postavi novu rotaciju;
		SetActorRotation(Rotation);
	}

}

void AItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
}

void AItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}