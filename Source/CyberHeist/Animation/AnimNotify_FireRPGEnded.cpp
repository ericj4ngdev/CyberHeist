// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_FireRPGEnded.h"

void UAnimNotify_FireRPGEnded::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		// 총의 재장전 함수 호출
		MeshComp->GetOwner();
	}
}
