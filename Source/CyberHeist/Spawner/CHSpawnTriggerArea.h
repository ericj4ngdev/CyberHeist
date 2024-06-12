// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CHEnemySpawner.h"
#include "AI/CHAIControllerBase.h"
#include "GameFramework/Actor.h"
#include "CHSpawnTriggerArea.generated.h"

UCLASS()
class CYBERHEIST_API ACHSpawnTriggerArea : public AActor
{
	GENERATED_BODY()
	
public:	
	ACHSpawnTriggerArea();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	
public:
	UPROPERTY(EditAnywhere)
	TArray<ACHEnemySpawner*> EnemySpawners;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBoxComponent> BoxCollisionComp;

	UPROPERTY(EditAnywhere)
	TArray<ACHAIControllerBase*> EnemyControllers;	

	void Respawn();
};
