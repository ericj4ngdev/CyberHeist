// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Gun/CHProjectile.h"
#include "CHProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHProjectileRocket : public ACHProjectile
{
	GENERATED_BODY()

public:
	ACHProjectileRocket();
	virtual void Destroyed() override;
	
protected:
	virtual void BeginPlay() override;

	// UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> ProjectileLoop;

	UPROPERTY()
	TObjectPtr<class UAudioComponent> ProjectileLoopComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundAttenuation> LoopingSoundAttenuation;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UProjectileMovementComponent> RocketMovementComponent;
};
