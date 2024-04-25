// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ACHProjectileRocket::ACHProjectileRocket()
{
	ProjectileMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Rocket Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RocketMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("RocketMovementComponent"));
	RocketMovementComponent->bRotationFollowsVelocity = true;
	// RocketMovementComponent->SetIsReplicated(true);
}


void ACHProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	/*if (!HasAuthority())
	{
	}*/
	/*if(CollisionComp->OnComponentHit.IsAlreadyBound(this,&ACHProjectileRocket::OnHit) == false)
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &ACHProjectileRocket::OnHit);		
	}*/

	SpawnTrailSystem();

	if (ProjectileLoop && LoopingSoundAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(
			ProjectileLoop,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			EAttachLocation::KeepWorldPosition,
			false,
			1.f,
			1.f,
			0.f,
			LoopingSoundAttenuation,
			(USoundConcurrency*)nullptr,
			false
		);
	}
}


void ACHProjectileRocket::Destroyed()
{
	Super::Destroyed();
}

void ACHProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse, const FHitResult& Hit)
{

	if(OtherActor == nullptr)
	{
		UE_LOG(LogTemp,Warning, TEXT("OtherActor = nullptr"));
		return;
	}
	if(HitComp == nullptr)
	{
		UE_LOG(LogTemp,Warning, TEXT("HitComp = nullptr"));
		return;
	}
	if(OtherComp == nullptr)
	{
		UE_LOG(LogTemp,Warning, TEXT("OtherComp = nullptr"));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("FDamageEvent"));
	UE_LOG(LogTemp, Warning, TEXT("OnHit"));
	UE_LOG(LogTemp, Warning, TEXT("HitComp : %s"), *HitComp->GetName());
	UE_LOG(LogTemp, Warning, TEXT("OtherActor : %s"), *OtherActor->GetName());
	UE_LOG(LogTemp, Warning, TEXT("OtherComp : %s"), *OtherComp->GetName());

	// DrawDebugBox(GetWorld(),)
	// USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(OtherComp); 
	// DrawDebugMesh(GetWorld(),SkeletalMeshComponent->)
	// Cast<USkeletalMeshComponent>(OtherComp);
	// DrawDebugCapsule(GetWorld(), HigherCapsuleOrigin, CheckRange, CheckCoverSphereRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawHighColor, false, 5.0f);
	
	
	if (OtherActor == GetOwner())
	{
		return;
	}
	ExplodeDamage();

	StartDestroyTimer();

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ProjectileMesh)
	{
		ProjectileMesh->SetVisibility(false);
	}
	if (CollisionComp)
	{
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (TrailSystemComponent && TrailSystemComponent->GetSystemInstanceController())
	{
		TrailSystemComponent->GetSystemInstanceController()->Deactivate();
	}
	if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}

	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}