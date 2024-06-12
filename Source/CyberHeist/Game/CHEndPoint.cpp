// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/CHEndPoint.h"

#include "CHGameMode.h"
#include "CyberHeist.h"
#include "Character/CHCharacterPlayer.h"
#include "Components/BoxComponent.h"

// Sets default values
ACHEndPoint::ACHEndPoint()
{
	// PrimaryActorTick.bCanEverTick = true;
	BoxCollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RootComponent = BoxCollisionComp;

	OverlappedPlayers = 0;
}

// Called when the game starts or when spawned
void ACHEndPoint::BeginPlay()
{
	CH_LOG(LogCHNetwork, Warning, TEXT("Begin"));
	Super::BeginPlay();
	BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ACHEndPoint::OnBeginOverlap);
	BoxCollisionComp->OnComponentEndOverlap.AddDynamic(this, &ACHEndPoint::OnEndOverlap);
}

void ACHEndPoint::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority())
	{
		CH_LOG(LogCHNetwork, Warning, TEXT("Begin"));
		// 플레이어 1, 2 확인하면 끝
		ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(OtherActor);
		if(CHPlayer && !CHPlayer->GetIsDead())
		{
			OverlappedPlayers++;
		}
		ACHGameMode* CHGameMode = Cast<ACHGameMode>(GetWorld()->GetAuthGameMode());
		if(CHGameMode && OverlappedPlayers == CHGameMode->MaxPlayers)
		{
			CHGameMode->WinCondition();		// 한번만 호출하면 되는데...
			BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// 플레이어 무적으로 만들기
		}
	}
	
}

void ACHEndPoint::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(HasAuthority())
	{
		ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(OtherActor);
		if(CHPlayer)
		{
			OverlappedPlayers--;
		}
	}
}




