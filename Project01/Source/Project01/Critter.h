#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Critter.generated.h" // uvek treba da bude poslednji include, inace ce biti error;

UCLASS()
class PROJECT01_API ACritter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACritter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//// static mesh naseg crittera;
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	//class UStaticMeshComponent *MeshComponent;
	// kul je static, ali hocemo i da se nesto mrda:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USkeletalMeshComponent *MeshComponent;
	// ona ima u sebi skeleton, moze da morphuje mesh;

	// kamera koja ce da bude attachovana za actora;
	UPROPERTY(EditAnywhere)
	class UCameraComponent *Camera;

	// intenzitet brzine upravljanja pawnom;
	UPROPERTY(EditAnywhere, Category = "Pawn Movement")
	float MaxSpeed;

private:
	// metode za kretanje pomocu user inputa
	void MoveForward(float Value);
	void MoveRight(float Value);

	// kontrola smera brzine upravljanja pawnom
	FVector CurrentVelocity;
};
