// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/CHCharacterStatComponent.h"
#include "Net/UnrealNetwork.h"
#include "CyberHeist.h"

// Sets default values for this component's properties
UCHCharacterStatComponent::UCHCharacterStatComponent()
{
	MaxHp = 100.0f;
	CurrentHp = MaxHp;

	bWantsInitializeComponent = true;

}

void UCHCharacterStatComponent::InitializeComponent()
{
	SetIsReplicated(true); 
	Super::InitializeComponent();

	SetHp(MaxHp);
}

void UCHCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);
	
	OnHpChanged.Broadcast(CurrentHp);
}

float UCHCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);
	//내가 받으려고하는 데미지 자체를 0 아래로 내려가지 않게 미연에 방지한다.
	SetHp(PrevHp - ActualDamage);
	//현재 HP 자체를 절대로 0아래로 내려가지 않게 하면서도 MaxHp보다 높게 잡히지 않게 만든다.
	if (CurrentHp <= KINDA_SMALL_NUMBER) 
	{
		//이제 Hp가 다 달았을 때, 다음과 같은 함수 호출.
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

void UCHCharacterStatComponent::BeginPlay()
{
	CH_SUBLOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Super::BeginPlay();
}

void UCHCharacterStatComponent::ReadyForReplication()
{
	CH_SUBLOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Super::ReadyForReplication();
}

void UCHCharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UCHCharacterStatComponent, CurrentHp);
}

void UCHCharacterStatComponent::OnRep_CurrentHp()
{
	CH_SUBLOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"));
	OnHpChanged.Broadcast(CurrentHp);
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}
}

