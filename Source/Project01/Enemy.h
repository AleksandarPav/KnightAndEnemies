// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

// enum za stanja u kojima se enemy nalazi
UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget UMETA(DisplayName = "MoveToTarget"),
	EMS_Attacking UMETA(DisplayName = "Attacking"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class PROJECT01_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	// da li ima koga da napada (treba nam false kad Main dumre)
	bool bHasValidTarget;

	// pocisti leseve iz scene
	void Disappear();

	// tajmer za cekanje do nestajanja sa scene
	FTimerHandle DeathTimer;

	// koliko da ceka kad dumre pre nego sto nestane
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DeathDelay;

	// da li je ziv
	bool Alive();

	// kad crkne dusman, sta da se radi
	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	// f-ja nasledjena iz Actora; parametri iskopirani
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	// f-ja za umiranje
	void Die(AActor *Causer);

	// DamageType klasa koja ce biti prosledjena u ApplyDamage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	// tajmer za cekanje izmedju enemyjevih zamaha
	FTimerHandle AttackTimer;

	// min i max vreme cekanja izmedju zamaha
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackTimeMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackTimeMax;

	// interpolating to the Main
	float InterpSpeed;
	bool bInterpToMain;
	void SetInterpToMain(bool Interp);

	// vraca ciljanu rotaciju prilikom interpolacije Enemyjeve rotacije ka Mainu,
	// koji se nalazi na lokaciji FVector Target
	FRotator GetLookAtRotationYaw(FVector Target);

	// indikator da li je enemy u rezimu napadanja
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttacking;

	// promenljiva za AnimMontage koji smo kreirali, pod imenom CombatMontage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	class UAnimMontage *CombatMontage;

	// box komponenta koja predstavlja deo enemyja koji pravi stetu
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent *CombatCollision;

	// zvuk koji ce da se cuje kad enemy bude hitovan
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue *HitSound;

	// zvuk koji ce da se cuje kad enemy zamahne
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	USoundCue *SwingSound;

	// particle effect kad igrac sibne enemyja
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UParticleSystem *HitParticles;

	// health i max health koju enemy poseduje
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxHealth;
	// damage za stetu koju enemy pravi igracu
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Damage;

	// target koji enemy treba da napada
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	class AMain *CombatTarget;

	// indikator preklapanja sa combat sferom
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingCombatSphere;

	// AIController koji nam treba da bismo koristili f-ju AIMoveTo
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController *AIController;

	// sfera koja sluzi da, kad se main sudari s njom, pauk krene da juri maina
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent *AgroSphere;

	// sfera koja sluzi da, kad se main sudari s njom, pauk krene da napada maina
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	USphereComponent *CombatSphere;

	// promenljiva za movement status
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	// seter za MovementStatus
	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }
	// geter za MovementStatus
	FORCEINLINE EEnemyMovementStatus GetEnemyMovementStatus() { return EnemyMovementStatus; }


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// f-ja za sta da se desi kad dodje do overlap izmedju agro sfere i maina;
	UFUNCTION()
	virtual void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	// f-ja za sta da se desi kad prestane overlap izmedju agro sfere i maina;
	UFUNCTION()
	virtual void AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	// f-ja za sta da se desi kad dodje do overlap izmedju combat sfere i maina;
	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	// f-ja za sta da se desi kad prestane overlap izmedju combat sfere i maina;
	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	// f-ja za kretanje AI ka Mainu
	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AMain *Target);

	// f-ja za sta da se desi kad pocne overlap izmedju combat box komponente na nozi i eneyja
	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	// f-ja za sta da se desi kad prestane overlap izmedju combat box komponente na nozi i enemyja
	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// f-je za (ne)omogucavanje kolizije
	UFUNCTION(BlueprintCallable)
	void ActivateCollision();
	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	// f-ja za aktiviranje montaze za napadanje
	void Attack();

	// regulise zavrsetak napada
	UFUNCTION(BlueprintCallable)
	void AttackEnd();
};
