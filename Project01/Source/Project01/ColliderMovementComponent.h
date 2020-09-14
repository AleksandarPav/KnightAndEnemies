#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ColliderMovementComponent.generated.h"

/*
	napravili sopstvenu klasu movement component, ali treba joj dodati funkcionalnosti;
	treba overrideovati TickComponent, koja se nalazi nekoliko nasledjivanja iznad UPawnMovementComponent;
 */
UCLASS()
class PROJECT01_API UColliderMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
	// paste-vana iz MovementComponent.h
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
};
