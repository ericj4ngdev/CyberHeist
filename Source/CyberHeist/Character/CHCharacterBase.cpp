// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CHCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon/Gun/CHGun.h"
#include "Character/CHCharacterControlData.h"
#include "Animation/AnimMontage.h"
#include "CharacterStat/CHCharacterStatComponent.h"
#include "UI/CHWidgetComponent.h"
#include "UI/CHHpBarWidget.h"
#include "Engine/DamageEvents.h"
#include "MotionWarpingComponent.h"
#include "Weapon/Gun/CHGunBase.h"


// Sets default values
ACHCharacterBase::ACHCharacterBase()
{
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	

	static ConstructorHelpers::FObjectFinder<UCHCharacterControlData> FirstPersonDataRef(TEXT("/Script/CyberHeist.CHCharacterControlData'/Game/CyberHeist/CharacterControl/CHC_FP.CHC_FP'"));
	if (FirstPersonDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::First, FirstPersonDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UCHCharacterControlData> ThirdPersonDataRef(TEXT("/Script/CyberHeist.CHCharacterControlData'/Game/CyberHeist/CharacterControl/CHC_TP.CHC_TP'"));
	if (ThirdPersonDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Third, ThirdPersonDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UCHCharacterControlData> FirstPersonAimDataRef(TEXT("/Script/CyberHeist.CHCharacterControlData'/Game/CyberHeist/CharacterControl/CHC_FP_Aim.CHC_FP_Aim'"));
	if (FirstPersonAimDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::FirstAim, FirstPersonAimDataRef.Object);
	}
	
	static ConstructorHelpers::FObjectFinder<UCHCharacterControlData> ThirdPersonAimDataRef(TEXT("/Script/CyberHeist.CHCharacterControlData'/Game/CyberHeist/CharacterControl/CHC_TP_Aim.CHC_TP_Aim'"));
	if (ThirdPersonAimDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::ThirdAim, ThirdPersonAimDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UCHCharacterControlData> ThirdPersonCoverDataRef(TEXT("/Script/CyberHeist.CHCharacterControlData'/Game/CyberHeist/CharacterControl/CHC_TP_Cover.CHC_TP_Cover'"));
	if (ThirdPersonCoverDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::ThirdCover, ThirdPersonCoverDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UCHCharacterControlData> ThirdPersonPrecisionAimDataRef(TEXT("/Script/CyberHeist.CHCharacterControlData'/Game/CyberHeist/CharacterControl/CHC_TP_PrecisionAim.CHC_TP_PrecisionAim'"));
	if (ThirdPersonPrecisionAimDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::ThirdPrecisionAim, ThirdPersonPrecisionAimDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UCHCharacterControlData> FirstPersonPrecisionAimDataRef(TEXT("/Script/CyberHeist.CHCharacterControlData'/Game/CyberHeist/CharacterControl/CHC_FP_PrecisionAim.CHC_FP_PrecisionAim'"));
	if (FirstPersonPrecisionAimDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::FirstScopeAim, FirstPersonPrecisionAimDataRef.Object);
	}
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> TakeCoverMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/CyberHeist/Animation/YBot_Retarget/AM_StartTakeCover.AM_StartTakeCover'"));
	if (TakeCoverMontageRef.Object) 
	{
		TakeCoverMontage = TakeCoverMontageRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/CyberHeist/Animation/AM_Dead.AM_Dead'"));
	if (DeadMontageRef.Object) 
	{
		DeadMontage = DeadMontageRef.Object;
	}

	//Stat Component 
	Stat = CreateDefaultSubobject<UCHCharacterStatComponent>(TEXT("Stat"));
	//WidgetComponent 
	HpBar = CreateDefaultSubobject<UCHWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));

	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/CyberHeist/UI/WBP_HpBar.WBP_HpBar_C"));

	if (HpBarWidgetRef.Class) {
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(150.0f, 15.0f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Motion
	MotionWarpComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));

	CurrentCharacterControlType = ECharacterControlType::Third;
}

// Called when the game starts or when spawned
void ACHCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Weapon = GetWorld()->SpawnActor<ACHGunBase>();
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	bCovered = false;

	SetbHasRPGInputBindings(false);
	SetbHasRifleInputBindings(false);
	SetbHasMinigunInputBindings(false);
}

USkeletalMeshComponent* ACHCharacterBase::GetFirstPersonMesh() const
{
	return FirstPersonMesh;	
}

USkeletalMeshComponent* ACHCharacterBase::GetThirdPersonMesh() const
{
	return GetMesh();
}

void ACHCharacterBase::SetCharacterControlData(const UCHCharacterControlData* CharacterControlData)
{
	// Pawn
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;
	bUseControllerRotationPitch = CharacterControlData->bUseControllerRotationPitch;

	// CharacteMovement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;

	
	
}

void ACHCharacterBase::SetMappingContextPriority(const UInputMappingContext* MappingContext, int32 Priority)
{
	
}

void ACHCharacterBase::Aim()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	AnimInstance->Montage_Play(AimActionMontage);
}

void ACHCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->OnHpZero.AddUObject(this, &ACHCharacterBase::SetDead);
}

float ACHCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	// UE_LOG(LogTemp, Log, TEXT("TakeDamage"));
	Stat->ApplyDamage(DamageAmount);

	return DamageAmount;
}

void ACHCharacterBase::AttackHitCheck()
{
	FHitResult OutHitResult;
	// Attack이란 태그로 우리가 수행한 이 작업에 대해서 조사할 수 있게 태그 추가
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const float AttackRange = 40.0f;
	const float AttackRadius = 50.0f;
	const float AttackDamage = 30.0f;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(AttackRadius), Params);

	// 감지
	if (HitDetected)
	{
		FDamageEvent DamageEvent;
		OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}
#if ENABLE_DRAW_DEBUG

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);

#endif

}

FName ACHCharacterBase::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}

uint8 ACHCharacterBase::IsInFirstPersonPerspective() const
{
	return bIsFirstPersonPerspective;
}

void ACHCharacterBase::SetAiming(uint8 bNewAiming)
{
	bAiming = bNewAiming;
	OnCombat.Broadcast(bAiming);
}

void ACHCharacterBase::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
}

void ACHCharacterBase::SetupCharacterWidget(UCHUserWidget* InUserWidget)
{
	UCHHpBarWidget* HpBarWidget = Cast<UCHHpBarWidget>(InUserWidget);
	if (HpBarWidget) 
	{
		HpBarWidget->SetMaxHp(Stat->GetMaxHp());
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());
		Stat->OnHpChanged.AddUObject(HpBarWidget, &UCHHpBarWidget::UpdateHpBar);
	}
}

void ACHCharacterBase::AddWeaponToInventory(ACHGunBase* NewWeapon, bool bEquipWeapon)
{
	if(NewWeapon == nullptr) return;
	Inventory.Weapons.Add(NewWeapon);
	SetCurrentWeapon(NewWeapon, CurrentWeapon);
}

void ACHCharacterBase::SetCurrentWeapon(ACHGunBase* NewWeapon, ACHGunBase* LastWeapon)
{
	if(NewWeapon == nullptr) return;
	if (NewWeapon == LastWeapon) return;

	// 총 교체시 중복임
	// 맨손이면 해제할 게 없는데??
	if(LastWeapon)
	{
		UnEquipWeapon(LastWeapon);		
	}

	if (NewWeapon)
	{
		// Weapons coming from OnRep_CurrentWeapon won't have the owner set
		CurrentWeapon = NewWeapon;
		CurrentWeapon->SetOwningCharacter(this);
		CurrentWeapon->Equip();

		// change gun animation
		UAnimMontage* Equip1PMontage = CurrentWeapon->GetEquip1PMontage();
		if (Equip1PMontage && GetFirstPersonMesh())
		{
			GetFirstPersonMesh()->GetAnimInstance()->Montage_Play(Equip1PMontage);
		}

		UAnimMontage* Equip3PMontage = CurrentWeapon->GetEquip3PMontage();
		if (Equip3PMontage && GetThirdPersonMesh())
		{
			UE_LOG(LogTemp, Log, TEXT("Equip3PMontage"));
			GetThirdPersonMesh()->GetAnimInstance()->Montage_Play(Equip3PMontage);
		}
	}
	else
	{
		// This will clear HUD, tags etc
		UnEquipWeapon(CurrentWeapon);
		CurrentWeapon = nullptr;
	}
	
}

void ACHCharacterBase::EquipWeapon(ACHGunBase* NewWeapon)
{
	SetCurrentWeapon(NewWeapon, CurrentWeapon);
}

void ACHCharacterBase::UnEquipWeapon(ACHGunBase* WeaponToUnEquip)
{
	if (WeaponToUnEquip)
	{		
		WeaponToUnEquip->UnEquip();
	}
}

ACHGunBase* ACHCharacterBase::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

void ACHCharacterBase::NextWeapon()
{
	// 인벤에 아무것도 없으면 
	if(Inventory.Weapons.Num() == 0) return;
	
	UE_LOG(LogTemp, Log, TEXT("NextWeapon"));
	int32 CurrentWeaponIndex = Inventory.Weapons.Find(CurrentWeapon);
	UnEquipWeapon(CurrentWeapon);

	int32 IndexOfNextWeapon = 0;
	if(Inventory.Weapons.Num() == 1)
	{
		IndexOfNextWeapon = CurrentWeaponIndex + 1;
	}
	else
	{
		IndexOfNextWeapon = (CurrentWeaponIndex + 1) % Inventory.Weapons.Num();
	}
	/*UE_LOG(LogTemp, Log, TEXT("CurrentWeaponIndex : %d"), CurrentWeaponIndex);
	UE_LOG(LogTemp, Log, TEXT("IndexOfNextWeapon : %d"), IndexOfNextWeapon);
	UE_LOG(LogTemp, Log, TEXT("Inventory.Weapons.Num() : %d"), Inventory.Weapons.Num());*/
	
	if(IndexOfNextWeapon >= Inventory.Weapons.Num())
	{
		CurrentWeapon = nullptr;
	}
	else
	{
		EquipWeapon(Inventory.Weapons[IndexOfNextWeapon]);		
	}
}

void ACHCharacterBase::PreviousWeapon()
{
	if(Inventory.Weapons.Num() == 0) return;
	UE_LOG(LogTemp, Log, TEXT("PreviousWeapon"));
	int32 CurrentWeaponIndex = Inventory.Weapons.Find(CurrentWeapon);
	UnEquipWeapon(CurrentWeapon);	

	int32 IndexOfPrevWeapon = FMath::Abs(CurrentWeaponIndex - 1 + Inventory.Weapons.Num()) % Inventory.Weapons.Num();
	if(IndexOfPrevWeapon <= 0)
	{
		CurrentWeapon = nullptr;		
	}
	else
	{		
		EquipWeapon(Inventory.Weapons[IndexOfPrevWeapon]);
	}	
}

void ACHCharacterBase::NotifyComboActionEnd()
{
}

void ACHCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);
	bIsDead = true;
	// HpBar->SetHiddenInGame(true);
}

void ACHCharacterBase::SetIsAttacking(uint8 IsAttack)
{
	bIsAttacking = IsAttack;
}

bool ACHCharacterBase::GetIsDead()
{
	return bIsDead;
}

uint8 ACHCharacterBase::GetIsAttacking()
{
	return bIsAttacking;
}

void ACHCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	if(DeadMontage)
	{
		AnimInstance->Montage_Play(DeadMontage, 1.0f);		
	}
	// UE_LOG(LogTemp, Log, TEXT("DeadMontage"));
}
