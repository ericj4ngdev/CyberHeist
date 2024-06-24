// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner/CHSpawnTriggerArea.h"

#include "CyberHeist.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BillboardComponent.h"
#include "AI/CHAIControllerBase.h"

// Sets default values
ACHSpawnTriggerArea::ACHSpawnTriggerArea()
{
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	RootComponent = SpriteComponent;
	// PrimaryActorTick.bCanEverTick = true;
	BoxCollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollisionComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACHSpawnTriggerArea::BeginPlay()
{
	CH_LOG(LogCHNetwork, Warning, TEXT("Begin"));
	Super::BeginPlay();

	BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ACHSpawnTriggerArea::OnBeginOverlap);
	if(HasAuthority())
	{		
		Respawn();
	}
	CH_LOG(LogCHNetwork, Warning, TEXT("End"));
}

void ACHSpawnTriggerArea::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	for (ACHAIControllerBase* Element : EnemyControllers)
	{
		Element->RunAI();
	}
}

void ACHSpawnTriggerArea::Respawn()
{
	EnemyControllers.Empty();
	for (ACHEnemySpawner* EnemySpawner : EnemySpawners)
	{
		EnemySpawner->SpawnEnemyWithWeapon();
		ACHAIControllerBase* EnemyController = Cast<ACHAIControllerBase>(EnemySpawner->SpawnedEnemy->GetController()); 
		EnemyControllers.Add(EnemyController);
	}
}
