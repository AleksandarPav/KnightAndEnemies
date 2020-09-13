// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
// header za MovementComponent;
#include "GameFramework/CharacterMovementComponent.h"

#include "Main.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	// Pawn ce biti nullptr very first time kad se pokrene ova f-ja, jer je u headeru samo deklarisan bez inicijalilzacije;
	if (Pawn == nullptr)
	{
		// get the pawn and set it to pawn;
		// f-ja vraca owner-a ove animation instance;
		Pawn = TryGetPawnOwner();
		
		// ako pawn validan, kastuj ga u Main charactera;
		if (Pawn)
			Main = Cast<AMain>(Pawn);
	}
}

void UMainAnimInstance::UpdateAnimationProperties()
{
	// za svaki slucaj proveri da li je Pawn validan, ali trebalo bi da jeste zbog NativeInitializeAnimation() f-je;
	if (Pawn == nullptr)
		// get the pawn and set it to pawn;
		// f-ja vraca owner-a ove animation instance;
		Pawn = TryGetPawnOwner();

	// ako validan
	if (Pawn)
	{
		// speed is something we can get from character's velocitiy;
		// vraca kojom brzinom se krece pawn u tom konkretnom frejmu;
		FVector Speed = Pawn->GetVelocity();
		// necemo da azuriramo brzinu u gornjem smeru ako je velocity sadrzi, samo u horizontalnoj ravni;
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		// MovementSpeed je float; Size vraca intenzitet vektora;
		MovementSpeed = LateralSpeed.Size();

		// proveri da li je u vazduhu;
		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		// ako je Main prazan ptr, kastuj Pawna u Main;
		if (Main == nullptr)
			Main = Cast<AMain>(Pawn);
	}
}