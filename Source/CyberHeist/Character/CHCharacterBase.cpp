// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CHCharacterBase.h"

#include "CyberHeist.h"
#include "EnhancedInputSubsystems.h"
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
#include "Animation/CHAnimInstance.h"
#include "Net/UnrealNetwork.h"
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

	bReplicates = true;
	bNearWall = false;
	CurrentCharacterControlType = ECharacterControlType::Third;
}

// Called when the game starts or when spawned
void ACHCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Weapon = GetWorld()->SpawnActor<ACHGunBase>();
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	bCovered = false;

	CHAnimInstance = Cast<UCHAnimInstance>(GetMesh()->GetAnimInstance());
	
	SetbHasRPGInputBindings(false);
	SetbHasRifleInputBindings(false);
	SetbHasMinigunInputBindings(false);	
}

void ACHCharacterBase::Tick(float DeltaSeconds)
{
	// Super::Tick(DeltaSeconds);	
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
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	// Pawn
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;
	bUseControllerRotationPitch = CharacterControlData->bUseControllerRotationPitch;
	bUseControllerRotationRoll = CharacterControlData->bUseControllerRotationRoll;
	
	// CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;

	CH_LOG(LogCHNetwork, Log, TEXT("End"))	
}

void ACHCharacterBase::SetMappingContextPriority(const UInputMappingContext* MappingContext, int32 Priority)
{
	
}

void ACHCharacterBase::OnRep_Aiming()
{
	// 애니메이션 변화? 
}

void ACHCharacterBase::OnRep_TPAimingCloser()
{
}

void ACHCharacterBase::OnRep_FPScopeAiming()
{
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

void ACHCharacterBase::SetAiming(uint8 bNewAiming)
{
	bAiming = bNewAiming;
	ServerSetAiming(bAiming);
	
	OnCombat.Broadcast(bAiming);		// UI
}

void ACHCharacterBase::SetTPAimingCloser(uint8 bNewTPAimingCloser)
{
	bTPAimingCloser = bNewTPAimingCloser;
	ServerSetTPAimingCloser(bTPAimingCloser);
}

void ACHCharacterBase::SetScopeAiming(uint8 bNewFPScopeAiming)
{
	bFPScopeAiming = bNewFPScopeAiming;
	ServerSetFPScopeAiming(bFPScopeAiming);
}

void ACHCharacterBase::ServerSetFPScopeAiming_Implementation(bool bNewFPScopeAiming)
{
	bFPScopeAiming = bNewFPScopeAiming;
}

void ACHCharacterBase::ServerSetTPAimingCloser_Implementation(bool bNewTPAimingCloser)
{
	bTPAimingCloser = bNewTPAimingCloser;
}

void ACHCharacterBase::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	GetCharacterMovement()->MaxWalkSpeed = bAiming ? SneakSpeed : WalkSpeed;
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

void ACHCharacterBase::OnRep_Inventory()
{
	CH_LOG(LogCHNetwork,Log,TEXT("Begin"))
	
	CH_LOG(LogCHNetwork,Log,TEXT("End"))
}

void ACHCharacterBase::AddWeaponToInventory(ACHGunBase* NewWeapon, bool bEquipWeapon)
{
	CH_LOG(LogCHNetwork,Log,TEXT("Begin"))
	if(NewWeapon == nullptr) return;
	// 애초에 ACHGunBase::OnSphereBeginOverlap에서 HasAuthority을 한다.
	// 즉, 이 함수는 서버에서만 실행된다. 
	if (HasAuthority()) // 서버에서만 인벤토리를 수정
	{
		Inventory.Weapons.Add(NewWeapon);
		// SetCurrentWeapon(NewWeapon, CurrentWeapon);		// 서버에서 호출이라 
		MultiCastRPCEquipWeapon(NewWeapon, CurrentWeapon);		// 클라에게 SetCurrentWeapon + OnRep함(해제, 장착)
	}
	CH_LOG(LogCHNetwork,Log,TEXT("End"))
}

void ACHCharacterBase::SetCurrentWeapon(ACHGunBase* NewWeapon, ACHGunBase* LastWeapon)
{
	CH_LOG(LogCHNetwork,Log,TEXT("Begin"))
	if(NewWeapon == nullptr) return;
	if(NewWeapon == LastWeapon) return;

	if(LastWeapon)
	{
		// UnEquipWeapon(LastWeapon);
		// LastWeapon->UnEquip();		// 서버에서만 일어남. 클라에서도 일어나야 함.
		// 멀티 캐스트?
		MulticastRPCUnEquipWeapon(LastWeapon);
	}

	if (NewWeapon)
	{
		CurrentWeapon = NewWeapon;		
		EquipWeapon();
	}
	else
	{
		// This will clear HUD, tags etc
		UnEquipWeapon(CurrentWeapon);
		CurrentWeapon = nullptr;
	}
	CH_LOG(LogCHNetwork,Log,TEXT("End"))
}

void ACHCharacterBase::OnRep_CurrentWeapon()
{
	CH_LOG(LogCHNetwork,Log,TEXT("Begin"))
	// EquipWeapon();
	CH_LOG(LogCHNetwork,Log,TEXT("End"))
}

void ACHCharacterBase::ServerRPCUnEquipWeapon_Implementation(ACHGunBase* LastWeapon)
{
	MulticastRPCUnEquipWeapon(LastWeapon);
}

bool ACHCharacterBase::ServerRPCUnEquipWeapon_Validate(ACHGunBase* LastWeapon)
{
	return true;
}

void ACHCharacterBase::MulticastRPCUnEquipWeapon_Implementation(ACHGunBase* LastWeapon)
{
	CH_LOG(LogCHNetwork,Log,TEXT("Begin"))
	if(LastWeapon)
	{
		LastWeapon->UnEquip();
		CurrentWeapon = nullptr;		
	} 
	CH_LOG(LogCHNetwork,Log,TEXT("End"))
}

void ACHCharacterBase::ServerRPCEquipWeapon_Implementation(ACHGunBase* NewWeapon, ACHGunBase* LastWeapon)
{
	CH_LOG(LogCHNetwork,Log,TEXT("Begin"))
	MultiCastRPCEquipWeapon(NewWeapon, LastWeapon);
	CH_LOG(LogCHNetwork,Log,TEXT("End"))
}

bool ACHCharacterBase::ServerRPCEquipWeapon_Validate(ACHGunBase* NewWeapon, ACHGunBase* LastWeapon)
{
	return true;
}

void ACHCharacterBase::MultiCastRPCEquipWeapon_Implementation(ACHGunBase* NewWeapon, ACHGunBase* LastWeapon)
{
	CH_LOG(LogCHNetwork,Log,TEXT("Begin"))
	if(NewWeapon == nullptr) return;
	if(NewWeapon == LastWeapon) return;

	if(LastWeapon)
	{
		UnEquipWeapon(LastWeapon);
	}

	if (NewWeapon)
	{
		CurrentWeapon = NewWeapon;		
		EquipWeapon();
	}
	else
	{
		// This will clear HUD, tags etc
		UnEquipWeapon(CurrentWeapon);
		CurrentWeapon = nullptr;
	}
	CH_LOG(LogCHNetwork,Log,TEXT("End"))
}

void ACHCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACHCharacterBase, CurrentWeapon);
	DOREPLIFETIME(ACHCharacterBase, Inventory);
	DOREPLIFETIME(ACHCharacterBase, bAiming);
	DOREPLIFETIME(ACHCharacterBase, bTPAimingCloser);	
	DOREPLIFETIME(ACHCharacterBase, bFPScopeAiming);
}

void ACHCharacterBase::EquipWeapon()
{
	CurrentWeapon->SetOwningCharacter(this);
	CurrentWeapon->Equip();

	UAnimMontage* Equip1PMontage = CurrentWeapon->GetEquip1PMontage();
	if (Equip1PMontage && GetFirstPersonMesh())
	{
		GetFirstPersonMesh()->GetAnimInstance()->Montage_Play(Equip1PMontage);
	}

	UAnimMontage* Equip3PMontage = CurrentWeapon->GetEquip3PMontage();
	if (Equip3PMontage && GetThirdPersonMesh())
	{
		GetThirdPersonMesh()->GetAnimInstance()->Montage_Play(Equip3PMontage);
	}
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
	CH_LOG(LogCHNetwork,Log,TEXT("Begin"))
	// 인벤에 아무것도 없으면 
	if(Inventory.Weapons.Num() == 0) return;
	
	UE_LOG(LogTemp, Log, TEXT("NextWeapon"));
	int32 CurrentWeaponIndex = Inventory.Weapons.Find(CurrentWeapon);
	// UnEquipWeapon(CurrentWeapon);

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
		ServerRPCUnEquipWeapon(CurrentWeapon);		
		// CurrentWeapon = nullptr;
	}
	else
	{
		ServerRPCEquipWeapon(Inventory.Weapons[IndexOfNextWeapon], CurrentWeapon);
		// SetCurrentWeapon(Inventory.Weapons[IndexOfNextWeapon], CurrentWeapon);
	}
	CH_LOG(LogCHNetwork,Log,TEXT("End"))
}

void ACHCharacterBase::PreviousWeapon()
{
	if(Inventory.Weapons.Num() == 0) return;
	UE_LOG(LogTemp, Log, TEXT("PreviousWeapon"));
	int32 CurrentWeaponIndex = Inventory.Weapons.Find(CurrentWeapon);
	// UnEquipWeapon(CurrentWeapon);

	int32 IndexOfPrevWeapon = FMath::Abs(CurrentWeaponIndex - 1 + Inventory.Weapons.Num()) % Inventory.Weapons.Num();
	if(IndexOfPrevWeapon <= 0)
	{
		ServerRPCUnEquipWeapon(CurrentWeapon);
		// CurrentWeapon = nullptr;
	}
	else
	{
		ServerRPCEquipWeapon(Inventory.Weapons[IndexOfPrevWeapon], CurrentWeapon);
		// SetCurrentWeapon(Inventory.Weapons[IndexOfPrevWeapon], CurrentWeapon);		
	}	
}

void ACHCharacterBase::MoveActorLocation(const FVector& Destination, float InterpSpeed)
{
	// 플레이어의 현재 위치와 목표 위치
	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = Destination; // 목표 위치 설정

	// 타이머 간격을 동적으로 가져오기
	float DeltaTime = FApp::GetDeltaTime();

	// 타이머를 이용하여 일정 간격으로 위치 업데이트
	FTimerHandle MovementTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, [&, CurrentLocation, TargetLocation]()
	{
		// 현재 위치에서 목표 위치로의 보간
		FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, InterpSpeed);

		// 새로운 위치 설정
		SetActorLocation(NewLocation);

		// 목표 위치에 도달했을 때 타이머 중지
		if (NewLocation.Equals(TargetLocation, 1.0f))
		{
			GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
		}
	}, DeltaTime, true);
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
