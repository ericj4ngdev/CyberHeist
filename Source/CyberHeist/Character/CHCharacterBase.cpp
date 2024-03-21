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
// #include "CharacterStat/CHCharaterStatComponent.h"

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

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/AssetPacks/ParagonWraith/Characters/Heroes/Wraith/Meshes/Wraith.Wraith'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/AssetPacks/ParagonWraith/Characters/Heroes/Wraith/Wraith_AnimBlueprint.Wraith_AnimBlueprint_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UCHCharacterControlData> FirstPersonDataRef(TEXT("/Script/CyberHeist.CHCharacterControlData'/Game/CyberHeist/CharacterControl/CHC_FirstPerson.CHC_FirstPerson'"));
	if (FirstPersonDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::First, FirstPersonDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UCHCharacterControlData> ThirdPersonDataRef(TEXT("/Script/CyberHeist.CHCharacterControlData'/Game/CyberHeist/CharacterControl/CHC_ThirdPerson.CHC_ThirdPerson'"));
	if (ThirdPersonDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Third, ThirdPersonDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UCHCharacterControlData> FirstPersonAimDataRef(TEXT("/Script/CyberHeist.CHCharacterControlData'/Game/CyberHeist/CharacterControl/CHC_FirstPerson_Aim.CHC_FirstPerson_Aim'"));
	if (FirstPersonAimDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::FirstAim, FirstPersonAimDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UCHCharacterControlData> ThirdPersonAimDataRef(TEXT("/Script/CyberHeist.CHCharacterControlData'/Game/CyberHeist/CharacterControl/CHC_ThirdPerson_Aim.CHC_ThirdPerson_Aim'"));
	if (ThirdPersonAimDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::ThirdAim, ThirdPersonAimDataRef.Object);
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
}

// Called when the game starts or when spawned
void ACHCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	Weapon = GetWorld()->SpawnActor<ACHGun>();
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	Weapon->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepRelativeTransform, TEXT("Weapon_rSocket"));
	Weapon->SetOwner(this);	
}

void ACHCharacterBase::SetCharacterControlData(const UCHCharacterControlData* CharacterControlData)
{
	// Pawn
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	// CharacteMovement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;

}

void ACHCharacterBase::Aim()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	AnimInstance->Montage_Play(AimActionMontage);
}

// Called every frame
void ACHCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACHCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACHCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->OnHpZero.AddUObject(this, &ACHCharacterBase::SetDead);
}

float ACHCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	/*float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply;*/
	// UE_LOG(LogTemp, Warning, TEXT("Health left %f"), Health);
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Stat->ApplyDamage(DamageAmount);

	return DamageAmount;
}

void ACHCharacterBase::SetupCharacterWidget(UCHUserWidget* InUserWidget)
{
	UCHHpBarWidget* HpBarWidget = Cast<UCHHpBarWidget>(InUserWidget);
	if (HpBarWidget) {
		HpBarWidget->SetMaxHp(Stat->GetMaxHp());
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());
		Stat->OnHpChanged.AddUObject(HpBarWidget, &UCHHpBarWidget::UpdateHpBar);
	}
}

void ACHCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);
	HpBar->SetHiddenInGame(true);
}

void ACHCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
	// UE_LOG(LogTemp, Log, TEXT("DeadMontage"));
}
