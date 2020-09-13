// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		// na ovaj nacin se dobija pawn koji ownuje ovu klasu
		Pawn = TryGetPawnOwner();
		if (Pawn)
			// castuj pawna u enemyja, tako ga imamo odmah na pocetku, u inicijalizaciji
			Enemy = Cast<AEnemy>(Pawn);
	}
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{
	// za svaki slucaj ponovo proveravamo da li je Pawn validan
	if (Pawn == nullptr)
		// na ovaj nacin se dobija pawn koji ownuje ovu klasu
		Pawn = TryGetPawnOwner();
	else
	{
		// speed is something we can get from character's velocitiy;
		// vraca kojom brzinom se krece pawn u tom konkretnom frejmu;
		FVector Speed = Pawn->GetVelocity();
		// necemo da azuriramo brzinu u gornjem smeru ako je velocity sadrzi, samo u horizontalnoj ravni;
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		// MovementSpeed je float; Size vraca intenzitet vektora;
		MovementSpeed = LateralSpeed.Size();

		// castuj pawna u enemyja, tako ga sigurno imamo ako vec ne postoji
		Enemy = Cast<AEnemy>(Pawn);
	}
}