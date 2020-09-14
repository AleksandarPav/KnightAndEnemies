#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class PROJECT01_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	// collision prostor za iteme
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Collision")
	class USphereComponent *CollisionVolume;

	// osnovna mesh komponenta za sve nasledjene klase;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Mesh")
	class UStaticMeshComponent *Mesh;

	// particle effects kad je item u mirovanju (IDLE) stanju;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	class UParticleSystemComponent *IdleParticlesComponent;

	// particle effects kad se desi overlap izmedju igraca i itema;
	// nije component, tako da nece biti kreiran pomocu CreateDefaultSubobject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	class UParticleSystem *OverlapParticles;

	// zvuk koji ce da se cuje kad se overlapuju igrac i item;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sounds")
	class USoundCue *OverlapSound;

	// indikator da li item treba ili ne treba da se rotira dok je u idle rezimu;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | ItemProperties")
	bool bRotate;
	// kojom brzinom da rotira ako treba;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | ItemProperties")
	float RotationRate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// f-ja za sta da se desi kad dodje do overlap izmedju itema i igraca;
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	// f-ja za sta da se desi kad prestane overlap izmedju itema i igraca;
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
