#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class PROJECT01_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	// mesh za floating platform
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
	class UStaticMeshComponent *Mesh;
	
	// startna pozicija za platformu;
	UPROPERTY(EditAnywhere)
	FVector StartPoint;

	// end pozicija za platformu; hocemo da imamo i mali widget za editovanje;
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector EndPoint;

	// brzina interpolacije; u .cpp se inicijalizuje na 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float InterpSpeed;

	// tajmer za platformu, da se zadrzava nekoliko sekundi pre nego sto promeni smer;
	FTimerHandle InterpTimer;
	// bool kao indikator da li da interpolira, ili stoji onih par sekundi u vazduhu;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	bool bInterping;
	
	// trajanje koliko ce platforma da bleji u vazduhu pre nego sto promeni smer; u .cpp se inicijalizuje na 3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float InterpTime;

	// distanca izmedju start i end pointa; sluzi za proveru koliko je platforma presla;
	float Distance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// zameni bool bInterping;
	void ToggleInterping();

	// swapuj vrednost dva vektora;
	void SwapVectors(FVector& V1, FVector& V2);
};
