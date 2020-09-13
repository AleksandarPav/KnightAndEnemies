// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Collider.generated.h"

UCLASS()
class PROJECT01_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Static Mesh - kako se collider vidi;
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* MeshComponent;

	// sfera za sudare, fiziku i sl.; moze a i ne mora da bude vidljiva u igri (checkira se u BP Editoru);
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USphereComponent* SphereComponent;

	// kamera koja prati pawna;
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UCameraComponent* Camera;
	// kontrola pokreta kamere;
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USpringArmComponent* SpringArm;

	// movement component koji je tipa ptr na kreiranu ColliderMovementComponent;
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UColliderMovementComponent* OurMovementComponent;

	// geter za movement component; vec postoji u klasi UPawnMovementComponent, zato override;
	virtual UPawnMovementComponent* GetMovementComponent() const override;

	// geter za static mesh;
	// sa kratkim funkcijama, zgodno je koristiti inline;
	// gde god da kompajler naidje na ovu f-ju u kodu, pasteovace definiciju (kao Macro), umesto da skace na mesto gde je
	// definisana - zato malo brze;
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() { return MeshComponent; }
	// seter za static mesh;
	// sa kratkim funkcijama, zgodno je koristiti inline;
	// gde god da kompajler naidje na ovu f-ju u kodu, pasteovace definiciju (kao Macro), umesto da skace na mesto gde je
	// definisana - zato malo brze;
	FORCEINLINE void SetMeshComponent(UStaticMeshComponent* Mesh) { MeshComponent = Mesh; }
	// geter i seter za sphere component;
	FORCEINLINE USphereComponent* GetSphereComponent() { return SphereComponent; }
	FORCEINLINE void SetSphereComponent(USphereComponent* Sphere) { SphereComponent = Sphere; }

	// geter i seter za kameru
	FORCEINLINE UCameraComponent* GetCameraComponent() { return Camera; }
	FORCEINLINE void SetCameraComponent(UCameraComponent* InCamera) { Camera = InCamera; }
	// geter i seter za spring arm;
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() { return SpringArm; }
	FORCEINLINE void SetSpringArmComponent(USpringArmComponent* InSpringArm) { SpringArm = InSpringArm; }

private:
	// metode za kretanje pomocu user inputa
	void MoveForward(float Input);
	void MoveRight(float Input);

	// metode za rotaciju prilikom okretanja misa;
	void PitchCamera(float AxisValue);
	void YawCamera(float AxisValue);

	// za rotiranje misem; ne trebaju nam 3 dimenzije;
	FVector2D CameraInput;
};
