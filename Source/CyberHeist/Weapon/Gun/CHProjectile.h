// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/CHCharacterBase.h"
#include "CHProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class CYBERHEIST_API ACHProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ACHProjectile();

	virtual void Destroyed() override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USceneComponent> SceneComponent;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent> CollisionComp;
	
	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> TrailSystemComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> ProjectileMesh;

	UPROPERTY()
	TObjectPtr<class UParticleSystemComponent> TracerComponent;
	
	UBoxComponent* GetCollisionComp() const { return CollisionComp; }
	
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovementComponent; }
	
	UPROPERTY(EditAnywhere, Category = "Setting")
	float InitialSpeed;

	// Only set this for Grenades and Rockets
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	// Doesn't matter for Grenades and Rockets
	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 40.f;

protected:
	virtual void BeginPlay() override;
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void SpawnTrailSystem();
	void ExplodeDamage();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UParticleSystem> ImpactParticles;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> ImpactSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraSystem> TrailSystem;
	
	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = 200.f;

	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = 500.f;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> Tracer;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;
};
