// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CHAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"	
#include "Character/CHCharacterBase.h"
#include "Kismet/GameplayStatics.h"


UCHAnimInstance::UCHAnimInstance()
{
	// 1인칭은 기준이 0도
	// 3인칭은 기준이 90도
	// 그래서 1인칭은 3인칭 - 90 하기 
	TiltAngle = 90.f;

	// 3인칭
	// 왼쪽 틸팅 70
	// 오른쪽 틸팅 110
	
	// 1인칭
	// 왼쪽 틸팅 -20
	// 오른쪽 틸팅 20
}

void UCHAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		CharacterMovement = Owner->GetCharacterMovement();
	}
	ACHCharacterBase* OwnerActor = Cast<ACHCharacterBase>(Owner);
	if(OwnerActor)
	{
		OwnerActor->OnCoverState.AddUObject(this, &UCHAnimInstance::SetCoverState);
	}
}

void UCHAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (CharacterMovement)
	{
		Velocity = CharacterMovement->Velocity;
		GroundSpeed = Velocity.Size2D();
		bIsFalling = CharacterMovement->IsFalling();
		bIsCrouching = CharacterMovement->IsCrouching();
		// FVector temp = CharacterMovement->GetCurrentAcceleration();
		// TODO: Compare Acceleration with Zero Vector 
		bShouldMove = (GroundSpeed > 3); // && temp == 0;	
		//&& !temp.Equals(FVector::Zero(), 0.1f);
	}
	
	ACHCharacterBase* OwnerActor = Cast<ACHCharacterBase>(Owner);
	if (OwnerActor)
	{
		bAiming = OwnerActor->GetAiming();
		bPrecisionAiming = OwnerActor->GetTPAimingCloser();
		bWithholdWeapon = OwnerActor->GetNearWall();
		bScopeAiming = OwnerActor->GetScopeAiming();
		TiltAngle = OwnerActor->GetTiltAngle();
		
		if(OwnerActor->GetCurrentWeapon())
		{
			CurrentWeapon = OwnerActor->GetCurrentWeapon()->WeaponType;	
		}
		else
		{
			CurrentWeapon = ECHWeaponType::UnArmed;
		}
	}
	
	// 1인칭 전용.. 이건 총마다 달라서 바꿔야 함.. 
	/*if(bAiming)
	{
		if(bScopeAiming) CurrentWeaponState = EWeaponState::RifleScopeAiming;
		else CurrentWeaponState = EWeaponState::RifleAiming;
	}
	else CurrentWeaponState = EWeaponState::Rifle;*/

	if (CurrentWeapon == ECHWeaponType::Rifle)
	{
		if(bAiming)
		{
			if(bScopeAiming) CurrentWeaponState = EWeaponState::RifleScopeAiming;
			else CurrentWeaponState = EWeaponState::RifleAiming;
		}
		else CurrentWeaponState = EWeaponState::Rifle;
	}
	else if(CurrentWeapon == ECHWeaponType::RPG)
	{
		if(bAiming)
		{
			if(bScopeAiming) CurrentWeaponState = EWeaponState::RPGScopeAiming;
			else CurrentWeaponState = EWeaponState::RPGAiming;
		}
		else CurrentWeaponState = EWeaponState::RPG;
	}
	else if(CurrentWeapon == ECHWeaponType::MiniGun)
	{
		if(bAiming)
		{
			CurrentWeaponState = EWeaponState::MiniGunAiming;
		}
		else CurrentWeaponState = EWeaponState::MiniGun;
	}
	else if(CurrentWeapon == ECHWeaponType::UnArmed)
	{
		CurrentWeaponState = EWeaponState::Fist;
	}
}

void UCHAnimInstance::SetHighCover(uint8 TakeHighCover)
{
	bTakeHighCover = TakeHighCover;
}

void UCHAnimInstance::SetLowCover(uint8 TakeLowCover)
{
	bTakeLowCover = TakeLowCover;
}

void UCHAnimInstance::SetCoverState(uint8 TakeHighCover, uint8 TakeLowCover)
{
	bTakeHighCover = TakeHighCover;
	bTakeLowCover = TakeLowCover;

	if(TakeHighCover && TakeLowCover)
	{
		CurrentCoverState = ECoverState::High;
	}
	else if(TakeHighCover == false && TakeLowCover)
	{
		CurrentCoverState = ECoverState::Low;
	}
	else
	{
		CurrentCoverState = ECoverState::None;
	}
}

void UCHAnimInstance::SetCoveredDirection(uint8 bRight)
{
	bCoverMoveRight = bRight;
}

void UCHAnimInstance::Recoil(float Multiplier)
{
	float LocalMultiplier;
	FVector RecoilLocation;
	FQuat RecoilRotationQuat; // FRotator 대신 FQuat을 사용합니다.

	float RRoll = UKismetMathLibrary::RandomFloatInRange(-2.5f, -5.0f);
	float RPitch = UKismetMathLibrary::RandomFloatInRange(-0.8f, -0.8f);
	float RYaw = UKismetMathLibrary::RandomFloatInRange(-1.6f, -1.6f);

	LocalMultiplier = Multiplier;
	// RecoilRotation = RecoilTransform.Rotator();
	// RecoilRotation = FRotator(RRoll, RPitch, RYaw) * LocalMultiplier;
	RecoilRotationQuat = FQuat(FRotator(RRoll, RPitch, RYaw)) * LocalMultiplier; // FRotator를 FQuat으로 변환합니다.

	float RX = UKismetMathLibrary::RandomFloatInRange(-0.16f, 0.16f);
	float RY = UKismetMathLibrary::RandomFloatInRange(-1.1f, -2.1f);
	float RZ = UKismetMathLibrary::RandomFloatInRange(-0.0f, -0.0f);

	RecoilLocation = FVector(RX, RY, RZ) * LocalMultiplier;

	// RecoilTransform.SetLocation(RecoilLocation);
	// RecoilTransform.SetRotation(RecoilRotationQuat);
}

void UCHAnimInstance::SetTilt()
{
	// 각도를 업데이트
}
