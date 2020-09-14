#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

/**
 * 
 */

// enum za razlicita stanja u kojem se weapon nalazi
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Pickup UMETA(DisplayName = "Pickup"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),

	EWS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class PROJECT01_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	
	AWeapon();

	// ime koje hocemo da sacuvamo prilikom cuvanja igre
	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	FString Name;

	// parametar za ApplyDamage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	// u ApplyDamage ide i instigator koji je tipa AController*;
	// Weapon nema Controllera jer je samo Actor, ali koristicemo Maina da getujemo Controller
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AController *WeaponInstigator;

	FORCEINLINE void SetInstigator(AController *Inst) { WeaponInstigator = Inst; }

	// f-je za (ne)omogucavanje kolizije
	UFUNCTION(BlueprintCallable)
	void ActivateCollision();
	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	// koliku stetu oruzje da pravi
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	float Damage;

	// box komponenta koja definise prostor oko maca s kojim se, ako se enemy sudari, pravi steta
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat")
	class UBoxComponent *CombatCollision;

	// stanje u kojem se nalazi weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	EWeaponState WeaponState;

	// seter za WeaponState
	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
	// geter za WeaponState
	FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }

	// mesh za weapon; downloadovani weapons nisu static, nego skeletal meshevi;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
	class USkeletalMeshComponent *SkeletalMesh;

	// f-ja za sta da se desi kad pocne overlap izmedju itema i igraca; nasledjena iz Itema;
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	// f-ja za sta da se desi kad prestane overlap izmedju itema i igraca; nasledjena iz Itema;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	// f-ja ce prosledjenog igraca da opremi ovim oruzjem;
	void Equip(class AMain *Character);

	// zvuk koji ce da se cuje kad se igrac opremi oruzjem
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sounds")
	class USoundCue *OnEquipSound;

	// zvuk koji ce da se cuje kad igrac swinguje macem
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sounds")
	USoundCue *SwingSound;

	// da li ce mac da ima ili nema particle effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	bool bWeaponParticleEffect;

	// f-ja za sta da se desi kad pocne overlap izmedju combat komponente i eneyja
	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	// f-ja za sta da se desi kad prestane overlap izmedju combat componente i enemyja
	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:
	// override Itemove BeginPlay();
	// treba nam za bindovanje overlapovanja sa nasim f-jama za overlapovanje
	virtual void BeginPlay() override;
};
