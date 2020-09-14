#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Explosive.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT01_API AExplosive : public AItem
{
	GENERATED_BODY()

public:
	AExplosive();

	// DamageType klasa koja ce biti prosledjena u ApplyDamage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	// f-ja za sta da se desi kad dodje do overlap izmedju itema i igraca;
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	// f-ja za sta da se desi kad prestane overlap izmedju itema i igraca;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	// koliku ce stetu da pravi explosive kad se overlapuje s igracem
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float Damage;
};
