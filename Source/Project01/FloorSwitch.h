// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

UCLASS()
class PROJECT01_API AFloorSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorSwitch();

	// zapremina komponente koja treba da trigeruje neki event;
	// overlap volume for funcionality to be triggered;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	class UBoxComponent *TriggerBox;

	// switch for the character to step on;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	class UStaticMeshComponent *FloorSwitch;
	
	// door to move when the switch is stepped on;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	// ne mora forward declase sa class, jer vec uradjen za FloorSwitch;
	UStaticMeshComponent *Door;

	// da se rastereti event graph od FloorSwitchBP; u ovaj vektor cuvana pocetna lokacija vrata;
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	FVector InitialDoorLocation;

	// u ovaj vektor cuvana pocetna lokacija switcha;
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	FVector InitialSwitchLocation;

	// timer za odbrojavanje vrmeena kad se skloni sa triggerboxa;
	FTimerHandle SwitchHandle;

	// koliko vremena da prodje od sklanjanja sa switcha do spustanja vrata i podizanja switcha
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	float SwitchTime;

	// indikator da li igrac stoji na switchu ili ne;
	bool bCharacterOnSwitch;

	// f-ja koja se poziva kad istekne timer;
	void CloseDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// BlueprintImplementableEvent znaci da ne moramo da pruzimo implementaciju u C++, moze da se uradi u Blueprints;
	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	// kad se desi overlap igraca sa trigger boxom, hocemo da se dignu vrata;
	void RaiseDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	// kad prestane overlap igraca sa trigger boxom, hocemo da se spuste vrata;
	void LowerDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	// kad se desi overlap igraca sa trigger boxom, hocemo da se spusti switch;
	void LowerFloorSwitch();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	// kad prestane overlap igraca sa trigger boxom, hocemo da se digne switch;
	void RaiseFloorSwitch();

	// hocemo da f-ja moze da se pozove iz BP; updateovace lokaciju vrata na osnovu parametra koji prima iz Timelina u BP;
	UFUNCTION(BlueprintCallable, Category = "Floor Switch")
	void UpdateDoorLocation(float Z);

	// hocemo da f-ja moze da se pozove iz BP; updateovace lokaciju switcha na osnovu parametra koji prima iz Timelina u BP;
	UFUNCTION(BlueprintCallable, Category = "Floor Switch")
	void UpdateFloorSwitchLocation(float Z);
};
