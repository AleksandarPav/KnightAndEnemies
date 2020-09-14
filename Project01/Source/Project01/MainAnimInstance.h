#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT01_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	// kao sto Actori imaju BeginPlay() koji se zove posle konstruktora, AnimInstance ima
	// NativeInitializeAnimation() koji je virtual i treba ga overridovati;
	virtual void NativeInitializeAnimation() override;

	// hocamo da f-ja azurira movement speed, is in air... svakog frejma;
	// hocemo da bude callable iz BP;
	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	void UpdateAnimationProperties();

	// treba nam movement speed da moze da update-uje BlendSpace, da zna da li idle, walk ili run;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeed;

	// da li je skocio;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsInAir;
	
	// referenca na naseg junaka;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class APawn *Pawn;

	// da bismo mogli da pristupimo propertijima Main-a;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class AMain *Main;
};
