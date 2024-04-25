// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Engine/DamageEvents.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CHPlayerController.h"
#include "Character/CHCharacterPlayer.h"
#include "CyberHeist/CyberHeist.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

ACHProjectile::ACHProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SetRootComponent(SceneComponent);
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->SetupAttachment(SceneComponent);

	
	// CollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	// CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// CollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	// CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	// CollisionComp->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);
}

void ACHProjectile::Destroyed()
{
	Super::Destroyed();
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}

void ACHProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached
		(
			Tracer,
			CollisionComp,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}

	if(ProjectileMovementComponent) ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	
	CollisionComp->OnComponentHit.AddDynamic(this, &ACHProjectile::OnHit);
	/*if (HasAuthority())
	{
	}*/
}

void ACHProjectile::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(
		DestroyTimer,
		this,
		&ACHProjectile::DestroyTimerFinished,
		DestroyTime
	);
}

void ACHProjectile::DestroyTimerFinished()
{
	Destroy();
}

void ACHProjectile::SpawnTrailSystem()
{
	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
}

void ACHProjectile::ExplodeDamage()
{
	APawn* FiringPawn = GetInstigator();
	//if (FiringPawn && HasAuthority())
	if (FiringPawn)
	{
		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this, // World context object
				Damage, // BaseDamage
				10.f, // MinimumDamage
				GetActorLocation(), // Origin
				DamageInnerRadius, // DamageInnerRadius
				DamageOuterRadius, // DamageOuterRadius
				1.f, // DamageFalloff
				UDamageType::StaticClass(), // DamageTypeClass
				TArray<AActor*>(), // IgnoreActors
				this, // DamageCauser
				FiringController // InstigatorController
			);
		}
	}
}

void ACHProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
	/*ACHCharacterPlayer* MyOwner = Cast<ACHCharacterPlayer>(GetOwner());
	if (MyOwner == nullptr) return;
	ACHPlayerController* OwnerInstigator = Cast<ACHPlayerController>(MyOwner->GetInstigatorController());
	if (OwnerInstigator == nullptr) return;

	if (OtherActor && OtherActor != this && OtherComp)
	{
		FDamageEvent DamageEvent;
		OtherActor->TakeDamage(Damage, DamageEvent, OwnerInstigator, this);

		if(OtherComp->IsSimulatingPhysics()) OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		// Destroy();		
	}*/
	// UE_LOG(LogTemp, Log, TEXT("OnHit"));	
}
