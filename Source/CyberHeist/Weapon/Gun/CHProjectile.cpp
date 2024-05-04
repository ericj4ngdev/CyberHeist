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

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SceneComponent);
	
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	// CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	// ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void ACHProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// DrawDebugBox(GetWorld(),CollisionComp->GetComponentLocation(),CollisionComp->GetScaledBoxExtent(), FColor::Red,false, 0.2f);
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
	
	// CollisionComp->OnComponentHit.AddDynamic(this, &ACHProjectile::OnHit);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ACHProjectile::OnHit);

	ECollisionEnabled::Type CollisionType = CollisionComp->GetCollisionEnabled();
	
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
				ExplosionDamage, // BaseDamage
				MinimumDamage, // MinimumDamage
				GetActorLocation(), // Origin
				DamageInnerRadius, // DamageInnerRadius
				DamageOuterRadius, // DamageOuterRadius
				DamageFalloff, // DamageFalloff
				UDamageType::StaticClass(), // DamageTypeClass
				TArray<AActor*>(), // IgnoreActors
				this, // DamageCauser
				FiringController // InstigatorController
			);
		}
	}
}

void ACHProjectile::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Destroy();	
}
