// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner/CHSpawnTriggerArea.h"

#include "CyberHeist.h"
#include "Components/BoxComponent.h"
#include "AI/CHAIControllerBase.h"

// Sets default values
ACHSpawnTriggerArea::ACHSpawnTriggerArea()
{ 	
	// PrimaryActorTick.bCanEverTick = true;
	BoxCollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RootComponent = BoxCollisionComp;
}

// Called when the game starts or when spawned
void ACHSpawnTriggerArea::BeginPlay()
{
	Super::BeginPlay();

	BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ACHSpawnTriggerArea::OnBeginOverlap);
	if(HasAuthority())
	{
		CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
		for (auto EnemySpawner : EnemySpawners)
		{
			EnemySpawner->SpawnEnemyWithWeapon();
			ACHAIControllerBase* EnemyController = Cast<ACHAIControllerBase>(EnemySpawner->SpawnedEnemy->GetController()); 
			EnemyControllers.Add(EnemyController);
		}
	}
}

void ACHSpawnTriggerArea::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	for (auto Element : EnemyControllers)
	{
		Element->RunAI();
	}
}

void ACHSpawnTriggerArea::Respawn()
{
	for (auto EnemySpawner : EnemySpawners)
	{
		if(EnemySpawner->SpawnedEnemy->GetIsDead())
		{
			EnemySpawner->SpawnEnemyWithWeapon();
		}
	}
}
