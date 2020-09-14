#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Pickup.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT01_API APickup : public AItem
{
	GENERATED_BODY()


public:
	APickup();

	// f-ja za sta da se desi kad dodje do overlap izmedju itema i igraca;
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	// f-ja za sta da se desi kad prestane overlap izmedju itema i igraca;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
	// f-ja koja ce biti implementirana u BPs, prosledjuje joj se Main s kojim se sudara
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnPickupBP(class AMain *Target);
};
