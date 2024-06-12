// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CHEndPoint.generated.h"

UCLASS()
class CYBERHEIST_API ACHEndPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACHEndPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBoxComponent> BoxCollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 OverlappedPlayers;
	
};
