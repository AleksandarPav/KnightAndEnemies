#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGame01.generated.h"

// struktura za podatke koje hocemo da sacuvamo
USTRUCT(BlueprintType)
struct FCharacterStats {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Health;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Stamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 Coins;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FVector Location;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString WeaponName;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString LevelName;
};


UCLASS()
class PROJECT01_API USaveGame01 : public USaveGame
{
	GENERATED_BODY()

public:

	USaveGame01();

	// za saveovanje
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString PlayerName;
	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;
	
	// struktura u kojoj smestene stvari za cuvanje
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FCharacterStats CharacterStats;
};
