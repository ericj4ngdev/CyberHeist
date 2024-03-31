// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CHPlayerController.h"
#include "Character/CHCharacterPlayer.h"


ACHProjectile::ACHProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(10.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ACHProjectile::OnHit);		// set up a notification for when this component hits something blocking
	// CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ACHProjectile::OnSphereBeginOverlap);

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

}

void ACHProjectile::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("OnSphereBeginOverlap"));
	//// Checking if it is a First Person Character overlapping
	//ACHCharacterBase* Character = Cast<ACHCharacterBase>(OtherActor);
	//if (Character != nullptr)
	//{
	//	FDamageEvent DamageEvent;
	//	Character->TakeDamage(Damage, DamageEvent, GetInstigatorController(), this);
	//	// Unregister from the Overlap Event so it is no longer triggered
	//	CollisionComp->OnComponentBeginOverlap.RemoveAll(this);
	//}
}


void ACHProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ACHCharacterPlayer* MyOwner = Cast<ACHCharacterPlayer>(GetOwner());
	if (MyOwner == nullptr) return;
	ACHPlayerController* OwnerInstigator = Cast<ACHPlayerController>(MyOwner->GetInstigatorController());
	if (OwnerInstigator == nullptr) return;

	if (OtherActor && OtherActor != this && OtherComp)
	{
		FDamageEvent DamageEvent;
		OtherActor->TakeDamage(Damage, DamageEvent, OwnerInstigator, this);
		// UE_LOG(LogTemp, Warning, TEXT("FDamageEvent"));
		// UE_LOG(LogTemp, Warning, TEXT("OnHit"));
		// UE_LOG(LogTemp, Warning, TEXT("HitComp : %s"), *HitComp->GetName());
		// UE_LOG(LogTemp, Warning, TEXT("OtherActor : %s"), *OtherActor->GetName());
		// UE_LOG(LogTemp, Warning, TEXT("OtherComp : %s"), *OtherComp->GetName());

		if(OtherComp->IsSimulatingPhysics()) OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		Destroy();		
	}

	//// Only add impulse and destroy projectile if we hit a physics
	//if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	//{		
	//	OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

	//	Destroy();
	//}

	

	// UE_LOG(LogTemp, Log, TEXT("OnHit"));
	

	//ACHCharacterBase* Character = Cast<ACHCharacterBase>(OtherActor);
	//if (Character != nullptr)
	//{
	//	FDamageEvent DamageEvent;
	//	Character->TakeDamage(Damage, DamageEvent, GetInstigatorController(), this);
	//	// Unregister from the Overlap Event so it is no longer triggered
	//	CollisionComp->OnComponentBeginOverlap.RemoveAll(this);
	//	UE_LOG(LogTemp, Warning, TEXT("OnHit"));
	//}

	
}
