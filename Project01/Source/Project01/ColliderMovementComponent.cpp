#include "ColliderMovementComponent.h"

/*
	napravili sopstvenu klasu movement component, ali treba joj dodati funkcionalnosti;
	treba overrideovati TickComponent, koja se nalazi nekoliko nasledjivanja iznad UPawnMovementComponent;
*/

// overrideovanje TickComponent;
void UColliderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	// kod virtualne f-je, u UE uvek treba zvati parent verziju, sto je omoguceno pomocu Super::;
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// treba proveriti par stvari koje su properties of the movement component;
	// check that everything is still valid and are allowed to move;
	// PawnOwner pripada MovementComponent klasi i mora da postoji; ako to nije slucaj, ne da nastavljas;
	// UpdatedComponent mora da bude valid;
	// vraca true ako treba preskociti update i tick component;
	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	// get and clear the vector from collider;
	// PeekDefinition na ConsumeInputVector(), samo ovo stoji:
	// return PawnOwner ? PawnOwner->Internal_ConsumeMovementInputVector() : FVector::ZeroVector;
	// ConsumeInputVector() "konzumira" vektor koji je prosledjen kao input (float Input u Collider-u) i onda ocisti vrednost vektora;
	// pritom, zbog GetClampeToMaxSize() ga clamp-uje na 1;
	// input, koji je u rasponu [-1, 1], pomnozi s nekom vrednoscu da se dobije pomeraj (vreme * brzina);
	FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f) * DeltaTime * 150.f;

	// ako vektor nije ~0, slobodan si nastaviti;
	if (!DesiredMovementThisFrame.IsNearlyZero())
	{
		// treba pozvati SafeMoveUpdatedComponent;
		// prima vektor delta - koliko hocemo da se pomeri pawn
		// rotaciju koju uzmemo od updated component,
		// bSweep - da li hocemo da se blokira prolazenje kroz druge objekte
		// FHitResult koji je napravljen zbog tog argumenta
		// i jos neki enum koji ima default vrednost tako da nije bitno;
		FHitResult Hit;
		SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);

		// ako bump into something, hocemo da slide-uje niz ivicu;
		if (Hit.IsValidBlockingHit())
		{
			// SlideAlongSurface prima vektor Delta - koliko da se pomeri,
			// vreme koje je udeo vektora Delta to apply, usually equal to the remaining time after a collision,
			// vektor koji je normalan na povrsinu sudara,
			// FHitResult koji smo malo pre kreirali
			// i neki bool koji je default false, tako da bmk da ga prosledjujem;
			SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
		}
	}
}