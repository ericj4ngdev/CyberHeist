// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CHCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Weapon/Gun/CHGun.h"
#include "CHCharacterControlData.h"
#include "Animation/CHAnimInstance.h"

ACHCharacterPlayer::ACHCharacterPlayer()
{
	// ThirdPersonCamera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = DefaultCameraDistance;
	// CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 0.0f);
	CameraBoom->bUsePawnControlRotation = true;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// ThirdPersonCamera->SetRelativeLocation()
	// (X = 75.000000, Y = 60.000000, Z = 50.000000)
	ThirdPersonCamera->bUsePawnControlRotation = false;

	// FirstPersonCamera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(),FName("Head"));		// ???
	FirstPersonCamera->bUsePawnControlRotation = true;
	// FirstPersonCamera->bAutoActivate = false;

	// Input
	static ConstructorHelpers::FObjectFinder<UInputAction> InputChangeActionControlRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_ChangeControl.IA_ChangeControl'"));
	if (nullptr != InputChangeActionControlRef.Object)
	{
		ChangeControlAction = InputChangeActionControlRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_Jump.IA_Jump'"));
	if (nullptr != InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionFirstPersonMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_FirstMove.IA_FirstMove'"));
	if (nullptr != InputActionFirstPersonMoveRef.Object)
	{
		FirstMoveAction = InputActionFirstPersonMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionFirstPersonLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_FirstLook.IA_FirstLook'"));
	if (nullptr != InputActionFirstPersonLookRef.Object)
	{
		FirstLookAction = InputActionFirstPersonLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionThirdPersonMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_ThirdMove.IA_ThirdMove'"));
	if (nullptr != InputActionThirdPersonMoveRef.Object)
	{
		ThirdMoveAction = InputActionThirdPersonMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionThirdPersonLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_ThirdLook.IA_ThirdLook'"));
	if (nullptr != InputActionThirdPersonLookRef.Object)
	{
		ThirdLookAction = InputActionThirdPersonLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShootRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_Shoot.IA_Shoot'"));
	if (nullptr != InputActionShootRef.Object)
	{
		ShootAction = InputActionShootRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAimRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_Aim.IA_Aim'"));
	if (nullptr != InputActionAimRef.Object)
	{
		AimAction = InputActionAimRef.Object;
	}
	
	// AimDistance = 100;
	CurrentCharacterControlType = ECharacterControlType::Third;
}

void ACHCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	SetCharacterControl(CurrentCharacterControlType);
}

void ACHCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::ChangeCharacterControl);
	EnhancedInputComponent->BindAction(FirstMoveAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::FirstMove);
	EnhancedInputComponent->BindAction(FirstLookAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::FirstLook);
	EnhancedInputComponent->BindAction(ThirdMoveAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::ThirdMove);
	EnhancedInputComponent->BindAction(ThirdLookAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::ThirdLook);
	
	EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::Shoot);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ACHCharacterPlayer::StartAim);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ACHCharacterPlayer::StopAim);
}

void ACHCharacterPlayer::ChangeCharacterControl()
{
	if (CurrentCharacterControlType == ECharacterControlType::ThirdAim) return;
	if (CurrentCharacterControlType == ECharacterControlType::First)
	{
		SetCharacterControl(ECharacterControlType::Third);
		ThirdPersonCamera->SetActive(true);
		CameraBoom->SetActive(true);
		FirstPersonCamera->SetActive(false);
	}
	else if (CurrentCharacterControlType == ECharacterControlType::Third)
	{
		SetCharacterControl(ECharacterControlType::First);
		FirstPersonCamera->SetActive(true);
		ThirdPersonCamera->SetActive(false);
		CameraBoom->SetActive(false);
	}
}

void ACHCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	UCHCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	// 컨트롤러 가져오기
	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	// IMC를 운영하는 subSystem을 가져온다. 
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		// 기존 IMC를 지우기 
		Subsystem->ClearAllMappings();
		// 새로운 Data에 있는 IMC로 바꿔치기
		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}
	CurrentCharacterControlType = NewCharacterControlType;   // 현재 enum값 교체
}

void ACHCharacterPlayer::SetCharacterControlData(const UCHCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	if (CurrentCharacterControlType == ECharacterControlType::Third || 
		CurrentCharacterControlType == ECharacterControlType::ThirdAim)
	{
		ThirdPersonCamera->SetRelativeLocation(CharacterControlData->CameraPosition);

		CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
		CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
		CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
		CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
		CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
		CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
		CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
	}
}

void ACHCharacterPlayer::Shoot()
{
	Weapon->PullTrigger();
}

void ACHCharacterPlayer::StartAim()
{
	if (CurrentCharacterControlType == ECharacterControlType::Third)
	{
		SetCharacterControl(ECharacterControlType::ThirdAim);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			UCHAnimInstance* CHAnimInstance = Cast<UCHAnimInstance>(AnimInstance);
			if (CHAnimInstance)
			{
				CHAnimInstance->SetCombatMode(true);
			}
		}

	}
}

void ACHCharacterPlayer::StopAim()
{
	if (CurrentCharacterControlType == ECharacterControlType::ThirdAim)
	{
		SetCharacterControl(ECharacterControlType::Third);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			UCHAnimInstance* CHAnimInstance = Cast<UCHAnimInstance>(AnimInstance);
			if (CHAnimInstance)
			{
				CHAnimInstance->SetCombatMode(false);
			}
		}
	}
}

void ACHCharacterPlayer::FirstMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ACHCharacterPlayer::FirstLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);		// modify
}

void ACHCharacterPlayer::ThirdMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ACHCharacterPlayer::ThirdLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);		// modify
}
