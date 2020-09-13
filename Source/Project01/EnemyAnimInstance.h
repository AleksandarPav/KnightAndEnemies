// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT01_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	// kao sto Actori imaju BeginPlay() koji se zove posle konstruktora, AnimInstance ima
	// NativeInitializeAnimation() koji je virtual i treba ga overridovati;
	virtual void NativeInitializeAnimation() override;

	// hocamo da f-ja azurira movement speed i animacije svakog frejma;
	// hocemo da bude callable iz BP;
	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	void UpdateAnimationProperties();

	// brzina enemyja
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeed;

	// referenca na pawna;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class APawn *Pawn;

	// da bismo mogli da pristupimo propertijima Enemyja;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class AEnemy *Enemy;
	
};
