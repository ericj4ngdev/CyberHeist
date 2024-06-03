// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CHCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Weapon/Gun/CHGun.h"
#include "CHCharacterControlData.h"
#include "UI/CHHUDWidget.h"
#include "Animation/CHAnimInstance.h"
#include "CharacterStat/CHCharacterStatComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/CHPlayerController.h"
#include "UI/UCHCrossHairWidget.h"
#include "CyberHeist.h"
#include "Net/UnrealNetwork.h"
#include "CHCharacterMovementComponent.h"


ACHCharacterPlayer::ACHCharacterPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCHCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// ThirdPersonCamera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = DefaultCameraDistance;
	CameraBoom->bUsePawnControlRotation = true;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	GetMesh()->bCastHiddenShadow = true;

	// Camera1PBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera1PBoom"));
	// Camera1PBoom->SetupAttachment(RootComponent);
	// Camera1PBoom->TargetArmLength
	
	// FirstPersonCamera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(RootComponent);	
	FirstPersonCamera->bUsePawnControlRotation = true;
	
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	FirstPersonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
	FirstPersonMesh->bReceivesDecals = false;
	FirstPersonMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	FirstPersonMesh->CastShadow = false;
	FirstPersonMesh->bCastHiddenShadow = false;
	FirstPersonMesh->SetVisibility(false, true);

	GetMesh()->bCastHiddenShadow = true;

	
	// Input
	static ConstructorHelpers::FObjectFinder<UInputAction> InputChangeActionControlRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_ChangeControl.IA_ChangeControl'"));
	if (nullptr != InputChangeActionControlRef.Object)
	{
		ChangePerspectiveControlAction = InputChangeActionControlRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_Jump.IA_Jump'"));
	if (nullptr != InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSprintRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_Sprint.IA_Sprint'"));
	if (nullptr != InputActionSprintRef.Object)
	{
		SprintAction = InputActionSprintRef.Object;
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

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionChangeNextWeaponActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_ChangeNextWeapon.IA_ChangeNextWeapon'"));
	if (nullptr != InputActionChangeNextWeaponActionRef.Object)
	{
		ChangeNextWeaponAction = InputActionChangeNextWeaponActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionChangePrevWeaponActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_ChangePrevWeapon.IA_ChangePrevWeapon'"));
	if (nullptr != InputActionChangePrevWeaponActionRef.Object)
	{
		ChangePrevWeaponAction = InputActionChangePrevWeaponActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionTakeCoverRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_Cover.IA_Cover'"));
	if (nullptr != InputActionTakeCoverRef.Object)
	{
		TakeCoverAction = InputActionTakeCoverRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionRightTiltRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_RightTilt.IA_RightTilt'"));
	if (nullptr != InputActionRightTiltRef.Object)
	{
		RightTiltAction = InputActionRightTiltRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLeftTiltRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_LeftTilt.IA_LeftTilt'"));
	if (nullptr != InputActionLeftTiltRef.Object)
	{
		LeftTiltAction = InputActionLeftTiltRef.Object;
	}


	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionCrouchRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_Crouch.IA_Crouch'"));
	if (nullptr != InputActionCrouchRef.Object)
	{
		CrouchAction = InputActionCrouchRef.Object;
	}
	
	
	CurrentCharacterControlType = ECharacterControlType::Third;
	bIsFirstPersonPerspective = false;

	bTiltReleaseLeft = false; 
	bTiltReleaseRight = false;
	// 캐릭터라서 이렇게 초기화
	CharacterHalfHeight = 96.f;
}

void ACHCharacterPlayer::BeginPlay()
{
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}
	// CharacterHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	// CharacterHalfHeight
	
	RadianForEscapeCover = FMath::DegreesToRadians(AngleForEscapeCover);
	UE_LOG(LogTemp, Log, TEXT("Cos(%f) : %f"), AngleForEscapeCover,FMath::Cos(AngleForEscapeCover));
	UE_LOG(LogTemp, Log, TEXT("Cos(%f) : %f"), RadianForEscapeCover,FMath::Cos(RadianForEscapeCover));
	// CHAnimInstance = Cast<UCHAnimInstance>(GetMesh()->GetAnimInstance());
	// InputVectorDirectionByCamera = 0.f;
	StartingThirdPersonMeshLocation = GetMesh()->GetRelativeLocation();
	StartingFirstPersonMeshLocation = FirstPersonMesh->GetRelativeLocation();
	// SetCharacterControl(CurrentCharacterControlType);

	// 클라만
	if (!HasAuthority())
	{
		SetPerspective(bIsFirstPersonPerspective);
		// ServerRPC_SetPerspective(bIsFirstPersonPerspective);
	}

	// Tilting Timeline Binding
	if (TiltingCurveFloat)
	{		
		FOnTimelineFloat TiltLeftTimelineProgress;
		FOnTimelineFloat TiltRightTimelineProgress;
		TiltLeftTimelineProgress.BindDynamic(this, &ACHCharacterPlayer::SetTiltingLeftValue);
		TiltRightTimelineProgress.BindDynamic(this, &ACHCharacterPlayer::SetTiltingRightValue);
		TiltingLeftTimeline.AddInterpFloat(TiltingCurveFloat, TiltLeftTimelineProgress);
		TiltingRightTimeline.AddInterpFloat(TiltingCurveFloat, TiltRightTimelineProgress);
	}
	CH_LOG(LogCHNetwork, Log, TEXT("End"))
}

void ACHCharacterPlayer::Tick(float DeltaTime)
{
	// CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	Super::Tick(DeltaTime);


	if (bIsFirstPersonPerspective)
	{
		TiltingLeftTimeline.TickTimeline(DeltaTime);
		TiltingRightTimeline.TickTimeline(DeltaTime);
	}

	if(IsLocallyControlled())
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);				// (1,0,0) 카메라 전방
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);					// (0,1,0) 
	
		DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(), GetActorLocation() + ForwardDirection * 100.0f, 10.0f, FColor::Cyan, false, -1, 0 ,5.0f);
		DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(), GetActorLocation() + RightDirection * 100.0f, 10.0f, FColor::Blue, false, -1, 0 ,5.0f);
	
		FVector Start_ = GetActorUpVector() * 10.0f + GetActorLocation();
		FVector End_ = GetActorForwardVector() * 50.0f + Start_;
		DrawDebugDirectionalArrow(GetWorld(),Start_, End_, 10.0f, FColor::Red, false, -1, 0 ,10.0f);
	}
	
	// CH_LOG(LogCHNetwork, Log, TEXT("End"))
	
}

void ACHCharacterPlayer::PossessedBy(AController* NewController)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s %s"), TEXT("Begin"), *GetName());
	AActor* OwnerActor = GetOwner();
	if (OwnerActor)
	{
		CH_LOG(LogCHNetwork, Log, TEXT("Owner : %s"), *OwnerActor->GetName());
	}
	else
	{
		CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("No Owner"));
	}

	Super::PossessedBy(NewController);
	OwnerActor = GetOwner();
	if (OwnerActor)
	{
		CH_LOG(LogCHNetwork, Log, TEXT("Owner : %s"), *OwnerActor->GetName());
	}
	else
	{
		CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("No Owner"));
	}

	CH_LOG(LogCHNetwork, Log, TEXT("%s %s"), TEXT("End"), *GetName());
}

void ACHCharacterPlayer::PostNetInit()
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s %s"), TEXT("Begin"), *GetName())
	
	Super::PostNetInit();

	AActor* OwnerActor = GetOwner();
    if (OwnerActor)
    {
    	CH_LOG(LogCHNetwork, Log, TEXT("Owner : %s"), *OwnerActor->GetName());
    }
    else
    {
    	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("No Owner"));
    }
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
}

void ACHCharacterPlayer::OnRep_Owner()
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s %s"), *GetName(),TEXT("Begin"))
	Super::OnRep_Owner();
	AActor* OwnerActor = GetOwner();
	if (OwnerActor)
	{
		CH_LOG(LogCHNetwork, Log, TEXT("Owner : %s"), *OwnerActor->GetName());
	}
	else
	{
		CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("No Owner"));
	}
	
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
}

void ACHCharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACHCharacterPlayer, bIsFirstPersonPerspective);
	DOREPLIFETIME(ACHCharacterPlayer, MoveDirection);
	DOREPLIFETIME(ACHCharacterPlayer, LastCoveredRotation);	
}

void ACHCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangePerspectiveControlAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::PressV);
	// EnhancedInputComponent->BindAction(ChangePerspectiveControlAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::ChangePerspectiveControlData);

	
	EnhancedInputComponent->BindAction(FirstMoveAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::FirstMove);
	EnhancedInputComponent->BindAction(FirstLookAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::FirstLook);
	
	EnhancedInputComponent->BindAction(ThirdMoveAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::ThirdMove);
	// EnhancedInputComponent->BindAction(ThirdCoveredMoveAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::ThirdCoveredMove);
	EnhancedInputComponent->BindAction(ThirdLookAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::ThirdLook);

	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACHCharacterPlayer::PressSprint);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACHCharacterPlayer::ReleaseSprint);

	EnhancedInputComponent->BindAction(ChangeNextWeaponAction, ETriggerEvent::Triggered, this, &ACHCharacterBase::NextWeapon);
	EnhancedInputComponent->BindAction(ChangePrevWeaponAction, ETriggerEvent::Triggered, this, &ACHCharacterBase::PreviousWeapon);

	EnhancedInputComponent->BindAction(TakeCoverAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::TakeCover);
	EnhancedInputComponent->BindAction(RightTiltAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::TiltRight);
	EnhancedInputComponent->BindAction(LeftTiltAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::TiltLeft);
	EnhancedInputComponent->BindAction(RightTiltAction, ETriggerEvent::Completed, this, &ACHCharacterPlayer::TiltRightRelease);
	EnhancedInputComponent->BindAction(LeftTiltAction, ETriggerEvent::Completed, this, &ACHCharacterPlayer::TiltLeftRelease);
	
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::TakeCrouch);
	// EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ACHCharacterPlayer::StopCrouch);
}

void ACHCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	// 다른 클라는 PlayerController가 없기 때문
	// 서버, 클라 본인 (다른 클라는 안됨)
	// if(HasAuthority() || IsLocallyControlled())
	UCHCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	UCHCharacterControlData* PrevCharacterControl = CharacterControlManager[CurrentCharacterControlType];

	// when player take covered in Fist personview and uncovered, new = Third exception
	if(PrevCharacterControl == CharacterControlManager[ECharacterControlType::First] && bCovered)
	{
		NewCharacterControl = CharacterControlManager[ECharacterControlType::First];
	}
	check(NewCharacterControl);

	// 클라만 바뀌게 하기
	// 시작이 3인칭

	// 클라가 3->1일 떄, 서버는 SetCharacterControlData 안하고 두 가지 속성 켜주기
	// 클라 본인은 SetCharacterControlData하기

	// 클라가 1->3일 때, 서버는 두가지 속성 꺼주기
	// 클라 본인은 SetCharacterControlData하기

	// 서버는 계속 3인칭이라 이걸로 구분 X
	// 클라만 구분하기
	// 서버는 CCD를 못바꾸게 막는다는 표현이 맞겠다.	
	
	if(IsLocallyControlled() && !HasAuthority())
	{
		// 클라만 CCD 변경
		SetCharacterControlData(NewCharacterControl);
	}
	else
	{
		// 클라가 아닌 다른 세상에서는
		// 클라가 3->1일 떄, 서버는 SetCharacterControlData 안하고 두 가지 속성 켜주기
		// 3인칭 엄폐->1일때는 왜 문제지?
		if(bIsFirstPersonPerspective)
		{
			CH_LOG(LogCHNetwork, Log, TEXT("1pp"))
			// 서버 CCD는 3인칭 엄폐.. 인가??
			bUseControllerRotationYaw = true;
			bUseControllerRotationPitch = true;
		}
		else
		{
			SetCharacterControlData(NewCharacterControl);
			GetCharacterMovement()->bOrientRotationToMovement = true;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
		}
	}
	

	// if(!HasAuthority())
	// if(!IsLocallyControlled())
	// if(!IsLocallyControlled())
	if(!IsLocallyControlled() && !HasAuthority())
	{
		CH_LOG(LogCHNetwork, Log, TEXT("Other Client"))
		return;
	}
	// ensure(GetController());
	if(GetOwner())
	{
		APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
		// Change IMC 
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{	
			UInputMappingContext* PrevMappingContext = PrevCharacterControl->InputMappingContext;
			UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
			
			if (PrevMappingContext)
			{
				Subsystem->RemoveMappingContext(PrevMappingContext);			
			}
		
			if (NewMappingContext)
			{
				Subsystem->AddMappingContext(NewMappingContext, 1);
			
			}
			// UE_LOG(LogTemp, Log, TEXT("Changed %s to %s"), *PrevMappingContext->GetName(), *NewMappingContext->GetName());
		}
		CurrentCharacterControlType = NewCharacterControlType;

		FString EnumAsString = UEnum::GetValueAsString<ECharacterControlType>(CurrentCharacterControlType);
		CH_LOG(LogCHNetwork, Log, TEXT("%s"), *EnumAsString)
	}
	else
	{
		CH_LOG(LogCHNetwork, Log, TEXT("No Controller"))
	}

	
	CH_LOG(LogCHNetwork, Log, TEXT("End"))
	// UE_LOG(LogTemp, Log, TEXT("CurrentCharacterControlType : %s"), CurrentCharacterControlType)
}

void ACHCharacterPlayer::SetCharacterControlData(const UCHCharacterControlData* CharacterControlData)
{
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	Super::SetCharacterControlData(CharacterControlData);

	FirstPersonCamera->bUsePawnControlRotation = CharacterControlData->bFPP_UsePawnControlRotation;
	ThirdPersonCamera->bUsePawnControlRotation = CharacterControlData->bTPP_UsePawnControlRotation;
	
	ThirdPersonCamera->SetRelativeLocation(CharacterControlData->TP_CameraPosition);
	FirstPersonCamera->SetRelativeLocation(CharacterControlData->FP_CameraPosition);
	ThirdPersonCamera->FieldOfView = CharacterControlData->TP_FieldOfView;
	FirstPersonCamera->FieldOfView = CharacterControlData->FP_FieldOfView;
	
	CameraBoom->SocketOffset = CharacterControlData->SocketOffset;	
	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
	CH_LOG(LogCHNetwork, Log, TEXT("End"))
}

void ACHCharacterPlayer::SetMappingContextPriority(const UInputMappingContext* MappingContext, int32 Priority)
{
	Super::SetMappingContextPriority(MappingContext, Priority);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());

	// Change IMC 
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{

		/*UInputMappingContext* PrevMappingContext = MappingContext;
		UInputMappingContext* NewMappingContext = MappingContext;
		/*if(PrevMappingContext->GetName() != NewMappingContext->GetName())
		{#1#
		if (PrevMappingContext)
		{
			Subsystem->RemoveMappingContext(PrevMappingContext);			
		}
		
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 1);
			
		}
		UE_LOG(LogTemp, Log, TEXT("Changed %s to %s"), *PrevMappingContext->GetName(), *NewMappingContext->GetName());*/
		// ---------------------------------------------------------
		
		// 없는 IMC를 제거할 수는 없으므로 캐릭터가 IMC가지고 있는지 체크하기  
		if(Subsystem->HasMappingContext(MappingContext))
		{
			Subsystem->RemoveMappingContext(MappingContext);
			Subsystem->AddMappingContext(MappingContext, Priority);
			// UE_LOG(LogTemp, Log, TEXT("SetMappingContextPriority"));
			// UE_LOG(LogTemp, Log, TEXT("Changed %s to %s"), *MappingContext->GetName(), *MappingContext->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("There is no MappingContext"));
		}
	}
}

void ACHCharacterPlayer::MulticastRPC_SetCharacterControl_Implementation(ECharacterControlType NewCharacterControlType)
{
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	
	SetCharacterControl(NewCharacterControlType);		
	
	CH_LOG(LogCHNetwork, Log, TEXT("End"))
}

void ACHCharacterPlayer::ServerRPC_SetCharacterControl_Implementation(ECharacterControlType NewCharacterControlType)
{
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	MulticastRPC_SetCharacterControl(NewCharacterControlType);
	CH_LOG(LogCHNetwork, Log, TEXT("End"))
}

bool ACHCharacterPlayer::ServerRPC_SetCharacterControl_Validate(ECharacterControlType NewCharacterControlType)
{
	return true;
}

void ACHCharacterPlayer::Jump()
{
	
	if(CurrentWeapon && CurrentWeapon->WeaponType == ECHWeaponType::MiniGun)
	{
		return;
	}

	if(bCovered)
	{
		return;
	}
	
	Super::Jump();
	
}

void ACHCharacterPlayer::FirstMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>().GetSafeNormal();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	float speed = bSprint ? RunSpeed : WalkSpeed;
	if(bAiming) speed = SneakSpeed;
	if(CurrentWeapon)
	{
		if(CurrentWeapon->WeaponType == ECHWeaponType::MiniGun)
		{
			speed = SneakSpeed;
		}		
	}
	
	AddMovementInput(ForwardDirection, MovementVector.Y * speed);
	AddMovementInput(RightDirection, MovementVector.X * speed);
	
	// GetCharacterMovement()->MaxWalkSpeed = speed;

	// UE_LOG(LogTemp, Log, TEXT("bSprint : %d	WalkSpeed : %f	RunSpeed : %f	speed : %f"), bSprint, WalkSpeed, RunSpeed, speed);
}

void ACHCharacterPlayer::FirstLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);		// modify
	// UE_LOG(LogTemp, Warning, TEXT("LookAxisVector.X : %f, LookAxisVector.Y : %f"), LookAxisVector.X, LookAxisVector.Y);	
}

void ACHCharacterPlayer::ThirdMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>().GetSafeNormal();
	// UE_LOG(LogTemp, Log, TEXT("MovementVector X: %f, Y: %f"), MovementVector.X, MovementVector.Y);

	if(bCovered)
	{
		LocalCoveredMove(Value);
	}
	else
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);	

		float speed = bSprint ? RunSpeed : WalkSpeed;
		if(bAiming) speed = WalkSpeed;
		if(bIsCrouched) speed = SneakSpeed;
		if(CurrentWeapon)
		{
			if(CurrentWeapon->WeaponType == ECHWeaponType::MiniGun)
			{
				speed = SneakSpeed;
			}		
		}
	
		// DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(), GetActorLocation() + ForwardDirection * 100.0f, 10.0f, FColor::Cyan, false, -1, 0 ,5.0f);
		// DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(), GetActorLocation() + RightDirection * 100.0f, 10.0f, FColor::Blue, false, -1, 0 ,5.0f);
	
		AddMovementInput(ForwardDirection, MovementVector.Y * speed);
		AddMovementInput(RightDirection, MovementVector.X * speed);
		//GetCharacterMovement()->MaxWalkSpeed = speed;
		// CH_LOG(LogCHNetwork, Log, TEXT("bSprint : %d MaxWalkSpeed : %f	speed : %f"), bSprint, GetCharacterMovement()->MaxWalkSpeed, speed)		
		// CH_LOG(LogCHNetwork, Log, TEXT("bSprint : %d	WalkSpeed : %f	RunSpeed : %f	speed : %f"), bSprint, WalkSpeed, RunSpeed, speed);		
	}	
}

void ACHCharacterPlayer::ThirdLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);		// modify
}

void ACHCharacterPlayer::LocalCoveredMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>().GetSafeNormal();
	
	const FVector ForwardVector = GetActorForwardVector();
	const FVector RightHand = GetActorRightVector();
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);				// (1,0,0) 카메라 벡터
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);					// (0,1,0)
	// UE_LOG(LogTemp, Log, TEXT("RightDirection X: %f, Y: %f | ForwardDirection X: %f, Y: %f"), RightDirection.X, RightDirection.Y, ForwardDirection.X, ForwardDirection.Y);
	// 오른쪽 벡터
	DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(), GetActorLocation() + RightHand * 100.f, 50.0f, FColor::Green, false, -1, 0 ,5.0f);
	const FVector InputVector = RightDirection * MovementVector.X + ForwardDirection * MovementVector.Y;
	// float AngleForCoveredMove_ = FVector::DotProduct(RightHand,InputVector);
	// 카메라 방향에 따른 입력값 부호(좌우. y값이 반영된 입력값 부호)
	InputVectorDirectionByCamera = 0.f;
	
	// case 1
	InputVectorDirectionByCamera = FVector::DotProduct(RightHand.GetSafeNormal(), InputVector.GetSafeNormal()) > 0 ? 1 : -1;
	// case 2
	/*InputVectorDirectionByCamera = FVector::DotProduct(RightHand.GetSafeNormal(), InputVector.GetSafeNormal());
	if(InputVectorDirectionByCamera > 0) InputVectorDirectionByCamera = 1;
	else if(InputVectorDirectionByCamera < 0) InputVectorDirectionByCamera = -1;*/
	
	// UE_LOG(LogTemp, Log, TEXT("InputVectorDirectionByCamera : %f"), InputVectorDirectionByCamera);
	// 각도에 따라 위치가 달라진다... 그냥 크기 1,0,-1로 고정해야 할 듯.
	float Range = 150.0f;
	float Radius = 5.0f;
	FVector Start = RightHand * InputVectorDirectionByCamera * 45.0f  + GetActorLocation() + GetActorUpVector() * GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.7f;			// 왼손, 오른손
	// UE_LOG(LogTemp, Log, TEXT("GetCapsuleComponent()->GetScaledCapsuleHalfHeight() : %f"), GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	// FVector Start = RightHand * AngleForDirection * 45.0f  + GetActorLocation();			// 왼손, 오른손
	FVector End = Start + (ForwardVector * Range);											// 벽쪽으로 레이저. 근데 - 안붙혀도 뒤로 나간다. 
	
	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("CoverTrace")), true, this);
	bool HitDetected = GetWorld()->SweepSingleByChannel(HitResult, Start, End,FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(Radius), TraceParams);
	// WallNormal = HitResult.ImpactNormal;
	
#if ENABLE_DRAW_DEBUG
		FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
		DrawDebugCapsule(GetWorld(), CapsuleOrigin, Range * 0.5f, Radius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);		
#endif

	CH_LOG(LogCHNetwork, Log, TEXT("HitDetected : %d"), HitDetected)
	// 벽의 끝에 도달. 
	if(HitDetected == false)
	{
		// 카메라 이동
		UCHCharacterControlData* NewCharacterControl = CharacterControlManager[ECharacterControlType::ThirdCover];
		CameraBoom->SocketOffset = FVector(NewCharacterControl->SocketOffset.X,NewCharacterControl->SocketOffset.Y * InputVectorDirectionByCamera,NewCharacterControl->SocketOffset.Z);
		return;
	}
	// 입력 벡터 
	float AngleForCoveredMove = FVector::DotProduct(HitResult.ImpactNormal,InputVector);
	DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(), GetActorLocation() + InputVector * 100.f, 50.0f, FColor::Emerald, false, -1, 0 ,5.0f);
	// 벽의 법선 벡터
	DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(), GetActorLocation() - HitResult.ImpactNormal * 100.f, 50.0f, FColor::Black, false, -1, 0 ,5.0f);
	
	FVector WallParallel = FVector(HitResult.ImpactNormal.Y, -HitResult.ImpactNormal.X, HitResult.ImpactNormal.Z);
	AngleForDirection = FVector::DotProduct(WallParallel,InputVector);		// y축 입력키도 반영하기 위한 내적
	
	// UE_LOG(LogTemp, Log, TEXT("Dir : %f "),Dir);
	// UE_LOG(LogTemp, Log, TEXT("AngleForDirection : %f "),AngleForDirection);
	// 40 degree ~ 180 degree = can cover
	// Degree Uproperty 
	if(AngleForCoveredMove >= -1.0f && AngleForCoveredMove <= FMath::Cos(RadianForEscapeCover))
	{
		// 서버 용 움직임을 만들어야 할 거 같은데
		// 매 틱마다 SetActorRotation은 아닌거 같다. 
		bCoverMoveRight = AngleForDirection > 0;
		ServerSetCoverMoveRight(bCoverMoveRight);
		MoveDirection = WallParallel * AngleForDirection;
		ServerSetMoveDirection(MoveDirection);
		CH_LOG(LogCHNetwork, Log, TEXT("MoveDirection : %s AngleForDirection : %f"), *MoveDirection.ToString(), AngleForDirection)
		SetActorRotation((-HitResult.ImpactNormal).Rotation());
		ServerSetActorRotation((-HitResult.ImpactNormal).Rotation());		// 여기서 LastCoveredRotation 갱신
		
		AddMovementInput(MoveDirection, SneakSpeed);
		
		// Want to = 법선 벡터 
		// current = 현재 내 액터의 rotation
	}
	else
	{
		bHighCovered = false;
		bLowCovered = false;
		// if(!HasAuthority()) OnCoverState.Broadcast(bHighCovered, bLowCovered);
		ServerSetCoverState(bHighCovered, bLowCovered);
		
		// Cancel Cover Anim Montage
		// OnCoverState.Broadcast(false, false);
		// UnCrouch();
		// 입력 속성 변경
		// Aim이면 조준상태 유지하면서 나오게 하기 위함
		if(CurrentCharacterControlType == ECharacterControlType::ThirdCover)
		{
			// SetCharacterControl(ECharacterControlType::Third);
			ServerRPC_SetCharacterControl(ECharacterControlType::Third);
		}			
		
		bCovered = false;
		UCHCharacterMovementComponent* CHMovement = Cast<UCHCharacterMovementComponent>(GetCharacterMovement());
		if(CHMovement)
		{
			CHMovement->SetCovered(bCovered);
		}
		UE_LOG(LogTemp, Log, TEXT("UnCovered"));
	}		
}

void ACHCharacterPlayer::TakeCover()
{	
	if(CurrentWeapon)
	{
		if(CurrentWeapon->WeaponType == ECHWeaponType::MiniGun) return;
	}
	
	// Not Covered 
	if(!bCovered)
	{
		StartCover();
	}
	else
	{
		StopCover();
	}	
}

void ACHCharacterPlayer::StartCover()
{
	// Take Cover
	// UE_LOG(LogTemp, Log, TEXT("CharacterHalfHeight : %f"), CharacterHalfHeight);
	FVector HighStart = FVector(GetActorLocation().X,  GetActorLocation().Y, GetActorLocation().Z) + GetActorUpVector() * CharacterHalfHeight * 0.5f;
	FVector LowerStart = FVector(GetActorLocation().X,  GetActorLocation().Y, GetActorLocation().Z) - GetActorUpVector() * CharacterHalfHeight * 0.5f;

	if(bIsCrouched)
	{
		HighStart = FVector(GetActorLocation().X,  GetActorLocation().Y,GetActorLocation().Z + CharacterHalfHeight * 0.55f) + GetActorUpVector() * CharacterHalfHeight * 0.5f;
		LowerStart = FVector(GetActorLocation().X,  GetActorLocation().Y,GetActorLocation().Z + CharacterHalfHeight * 0.55f) - GetActorUpVector() * CharacterHalfHeight * 0.5f;
	}
	// float Radius = CharacterHalfHeight * 0.5f;									// 구체의 반경 설정
	// CheckRange = 150.0f;													// 엄폐물 조사 반경
	FVector HighEnd = HighStart + GetActorForwardVector() * CheckRange;			// 높이를 설정하여 바닥으로 Sphere Trace
	FVector LowerEnd = LowerStart + GetActorForwardVector() * CheckRange;		// 높이를 설정하여 바닥으로 Sphere Trace

	FHitResult HitHighCoverResult;
	FCollisionQueryParams HighTraceParams(FName(TEXT("HighCoverTrace")), true, this);

	FHitResult HitLowCoverResult;
	FCollisionQueryParams LowTraceParams(FName(TEXT("LowCoverTrace")), true, this);
	
	bool HitHighCoverDetected = GetWorld()->SweepSingleByChannel(HitHighCoverResult, HighStart, HighEnd,
		FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(CheckCoverSphereRadius), HighTraceParams);
	bool HitLowCoverDetected = GetWorld()->SweepSingleByChannel(HitLowCoverResult, LowerStart, LowerEnd,
		FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(CheckCoverSphereRadius), LowTraceParams);
	
	// 감지
	if (HitLowCoverDetected)
	{
		// 엄폐 방향 결정
		FVector WallParallel = FVector(HitLowCoverResult.ImpactNormal.Y, -HitLowCoverResult.ImpactNormal.X, HitLowCoverResult.ImpactNormal.Z);
		FVector ActorForward = GetActorForwardVector();

		float Result = FVector::DotProduct(WallParallel, ActorForward);
		bCoverMoveRight = Result > 0;
		// if(!HasAuthority())
		// CHAnimInstance->SetCoveredDirection(bCoverMoveRight);
		ServerSetCoverMoveRight(bCoverMoveRight);
		// CHAnimInstance->SetCoveredDirection(Result > 0);			
		
		if(HitHighCoverDetected)
		{
			// Move to Cover
			FVector TargetLocation = HitLowCoverResult.Location;
			FRotator TargetRotation = UKismetMathLibrary::NormalizedDeltaRotator(HitLowCoverResult.ImpactNormal.Rotation(), FRotator(0.0f, 180.0f,0.0f));
			LastCoveredRotation = TargetRotation;
			ServerSetCoveredRotation(LastCoveredRotation);
			// 여기서 LastCoveredRotation을 갱신한다.
			// 그리고 LocalCoveredMove에서 움직일 때, LastCoveredRotation를 ServerSetActorRotation((-HitResult.ImpactNormal).Rotation()); 로 갱신
			// UE_LOG(LogTemp, Log, TEXT("Target Location: %s"), *TargetLocation.ToString());

			float Distance = FVector::Distance(TargetLocation,GetActorLocation());
			UE_LOG(LogTemp, Log, TEXT("Distance : %f"), Distance);
			
			if(Distance > 70.0f)
			{
				FMotionWarpingTarget Target;
				Target.Name = FName("StartTakeCover");				
				Target.Location = TargetLocation;
				Target.Rotation = TargetRotation;
				
				MotionWarpComponent->AddOrUpdateWarpTarget(Target);
				
				CHAnimInstance->StopAllMontages(0.0f);
				CHAnimInstance->Montage_Play(TakeCoverMontage, 1);									
			}/*
			else
			{
				SetActorRotation(LastCoveredRotation);
				ServerSetActorRotation(LastCoveredRotation);
			}*/
			// 도착하면 모션 멈추기...				
			// Crouch();
			// 엄폐 애니메이션
			bHighCovered = HitHighCoverDetected;
			bLowCovered = HitLowCoverDetected;
			// if(!HasAuthority()) OnCoverState.Broadcast(bHighCovered, bLowCovered);
			ServerSetCoverState(bHighCovered, bLowCovered);
			// OnCoverState.Broadcast(true,true);
			
			// 움직임 속도 제한
			UCHCharacterMovementComponent* CHMovement = Cast<UCHCharacterMovementComponent>(GetCharacterMovement());
			if(CHMovement)
			{
				CHMovement->SetCovered(bCovered);
			}
			// GetCharacterMovement()->MaxWalkSpeed = SneakSpeed;
			
			// 입력 속성 변경
			// SetCharacterControl(ECharacterControlType::ThirdCover);
			ServerRPC_SetCharacterControl(ECharacterControlType::ThirdCover);
			
			UE_LOG(LogTemp, Log, TEXT("High Covered"));
		}
		else
		{
			// Move to Cover
			FVector TargetLocation = HitLowCoverResult.Location; //  + HitLowCoverResult.ImpactNormal * 1.0f;				
			FRotator TargetRotation = UKismetMathLibrary::NormalizedDeltaRotator(HitLowCoverResult.ImpactNormal.Rotation(), FRotator(0.0f, 180.0f,0.0f));
			LastCoveredRotation = TargetRotation;
			ServerSetCoveredRotation(LastCoveredRotation);
			// UE_LOG(LogTemp, Log, TEXT("Target Location: %s"), *TargetLocation.ToString());

			float Distance = FVector::Distance(TargetLocation,GetActorLocation());
			UE_LOG(LogTemp, Log, TEXT("Distance : %f"), Distance);
			
			// Play Cover Anim Montage
			if(Distance > 70.0f)
			{
				FMotionWarpingTarget Target;
				Target.Name = FName("StartTakeCover");				
				Target.Location = TargetLocation;
				Target.Rotation = TargetRotation;

				MotionWarpComponent->AddOrUpdateWarpTarget(Target);
				
				CHAnimInstance->StopAllMontages(0.0f);
				CHAnimInstance->Montage_Play(TakeCoverMontage, 1);									
			}/*
			else
			{
				SetActorRotation(LastCoveredRotation);
				ServerSetActorRotation(LastCoveredRotation);
			}*/
			
			// 엄폐 애니메이션
			bHighCovered = HitHighCoverDetected;
			bLowCovered = HitLowCoverDetected;
			// if(!HasAuthority()) OnCoverState.Broadcast(bHighCovered, bLowCovered);
			ServerSetCoverState(bHighCovered, bLowCovered);
			// OnCoverState.Broadcast(false, true);
			Crouch();
			// 움직임 속도 제한
			UCHCharacterMovementComponent* CHMovement = Cast<UCHCharacterMovementComponent>(GetCharacterMovement());
			if(CHMovement)
			{
				CHMovement->SetCovered(bCovered);
			}
			// GetCharacterMovement()->MaxWalkSpeed = SneakSpeed;
			
			// 입력 속성 변경
			// 1인칭일 때는 무시
			// SetCharacterControl(ECharacterControlType::ThirdCover);
			ServerRPC_SetCharacterControl(ECharacterControlType::ThirdCover);
			
			UE_LOG(LogTemp, Log, TEXT("Low Covered"));
		}			
		bCovered = true;			
	}
	else
	{
		// UnCrouch();
		UE_LOG(LogTemp, Log, TEXT("Nothing to Cover"));			
	}
#if ENABLE_DRAW_DEBUG

	FVector HigherCapsuleOrigin = HighStart + (HighEnd - HighStart) * 0.5f;		// = GetActorForwardVector() * CheckRange
	FVector LowerCapsuleOrigin = LowerStart + (LowerEnd - LowerStart) * 0.5f;
	
	FColor DrawHighColor = HitHighCoverDetected ? FColor::Green : FColor::Red;		
	FColor DrawLowerColor = HitLowCoverDetected ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), HigherCapsuleOrigin, CheckRange, CheckCoverSphereRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawHighColor, false, 5.0f);
	DrawDebugCapsule(GetWorld(), LowerCapsuleOrigin, CheckRange, CheckCoverSphereRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawLowerColor, false, 5.0f);		
#endif
}

void ACHCharacterPlayer::StopCover()
{
	// Cancel Cover Anim Montage
	bHighCovered = false;
	bLowCovered = false;
	// if(!HasAuthority())
	// OnCoverState.Broadcast(bHighCovered, bLowCovered);
	ServerSetCoverState(bHighCovered, bLowCovered);
	// OnCoverState.Broadcast(false,false);

	// 입력 속성 변경(1인칭일 때는 변경 X)
	if(!bIsFirstPersonPerspective)
	{
		// SetCharacterControl(ECharacterControlType::Third);
		ServerRPC_SetCharacterControl(ECharacterControlType::Third);
	}
	
	bCovered = false;
	UE_LOG(LogTemp, Log, TEXT("UnCovered"));
}

void ACHCharacterPlayer::ReturnCover()
{
	CH_LOG(LogCHTemp, Log, TEXT("LastCoveredRotation : %s"), *LastCoveredRotation.ToString())
	
	CH_LOG(LogCHTemp, Log, TEXT("Before ActorRotation : %s"), *GetActorRotation().ToString())
	SetActorRotation(LastCoveredRotation);
	CH_LOG(LogCHTemp, Log, TEXT("After ActorRotation : %s"), *GetActorRotation().ToString())
	
}

void ACHCharacterPlayer::TakeCrouch()
{
	if(CHAnimInstance->GetCurrentCoverState() == ECoverState::Low)
	{
		// 1인칭 전환시 CoverState None으로 바꾸기
		// StopCover();로 해줌
		return;
	}
	if(bIsCrouched)
	{
		StopCrouch();
	}
	else
	{
		StartCrouch();		
	}
}

void ACHCharacterPlayer::StartCrouch()
{
	if(CurrentWeapon)
	{
		if(CurrentWeapon->WeaponType == ECHWeaponType::MiniGun) return;
	}
	Crouch();
	
}

void ACHCharacterPlayer::StopCrouch()
{
	if(CurrentWeapon)
	{
		if(CurrentWeapon->WeaponType == ECHWeaponType::MiniGun) return;
	}
	UnCrouch();
}

void ACHCharacterPlayer::ServerSetCoveredRotation_Implementation(FRotator NewCoveredRotation)
{
	LastCoveredRotation = NewCoveredRotation;
}

bool ACHCharacterPlayer::ServerSetCoveredRotation_Validate(FRotator NewCoveredRotation)
{
	return true;
}

void ACHCharacterPlayer::SetTiltingRightValue(const float Value)
{
	if (bTiltReleaseRight)
	{
		CameraCurrentPosition = FirstPersonCamera->GetRelativeLocation();
		CameraDesiredPosition = FVector(0,0,60);
		CameraCurrentRotation = FirstPersonCamera->GetRelativeRotation();
		CameraDesiredRotation = FRotator::ZeroRotator;
		
		CurrentTiltAngle = TiltAngle;
		DesiredTiltAngle = 0;
		CurrentTiltLocation = TiltLocation;
		DesiredTiltLocation = FVector(0,0,0);;		
	}
	else
	{
		CameraCurrentPosition = FirstPersonCamera->GetRelativeLocation();
		CameraDesiredPosition = FVector(0,50,60);
		CameraCurrentRotation = FirstPersonCamera->GetRelativeRotation();
		CameraDesiredRotation = FRotator(0, 0, 20);
		
		CurrentTiltAngle = TiltAngle;
		DesiredTiltAngle = 30;
		CurrentTiltLocation = TiltLocation;
		DesiredTiltLocation = FVector(-10,0,0);;
	}
	// UE_LOG(LogTemp, Log, TEXT("[SetTiltingRightValue] bTiltReleaseRight : %d"), bTiltReleaseRight)

	// RLerp와 TimeLine Value 값을 통한 자연스러운 기울이기
	const FRotator RLerp = UKismetMathLibrary::RLerp(CameraCurrentRotation, CameraDesiredRotation, Value, true);
	const FVector VLerp = UKismetMathLibrary::VLerp(CameraCurrentPosition, CameraDesiredPosition, Value);
	const FTransform TLerp = UKismetMathLibrary::MakeTransform(VLerp, RLerp);
	// 해당 트랜스폼 할당
	FirstPersonCamera->SetRelativeTransform(TLerp);

	// 애님에게 값 전달.
	const float TiltValue = UKismetMathLibrary::Lerp(CurrentTiltAngle, DesiredTiltAngle,Value);
	TiltAngle = TiltValue;
	ServerSetTiltAngle(TiltAngle);

	const FVector TiltLocationValue = UKismetMathLibrary::VLerp(CurrentTiltLocation, DesiredTiltLocation,Value);
	TiltLocation = TiltLocationValue;
	ServerSetTiltLocation(TiltLocation);
}

void ACHCharacterPlayer::SetTiltingLeftValue(const float Value)
{
	if (bTiltReleaseLeft)
	{
		// UE_LOG(LogTemp, Log, TEXT("[SetTiltingLeftValue] bTiltReleaseLeft : %d"), bTiltReleaseLeft);
		CameraCurrentPosition = FirstPersonCamera->GetRelativeLocation();
		CameraDesiredPosition = FVector(0,0,60);
		CameraCurrentRotation = FirstPersonCamera->GetRelativeRotation();
		CameraDesiredRotation = FRotator::ZeroRotator;

		CurrentTiltAngle = TiltAngle;
		DesiredTiltAngle = 0;
		CurrentTiltLocation = TiltLocation;
		DesiredTiltLocation =  FVector(0,0,0);
	}
	else
	{
		// UE_LOG(LogTemp, Log, TEXT("[SetTiltingLeftValue] bTiltReleaseLeft : %d"), bTiltReleaseLeft);
		// 눌렀을 때, 
		CameraCurrentPosition = FirstPersonCamera->GetRelativeLocation();
		CameraDesiredPosition = FVector(0,-50,60);
		CameraCurrentRotation = FirstPersonCamera->GetRelativeRotation();
		CameraDesiredRotation = FRotator(0, 0, -20);

		CurrentTiltAngle = TiltAngle;
		DesiredTiltAngle = -30;
		CurrentTiltLocation = TiltLocation;
		DesiredTiltLocation = FVector(10,0,0);
	}
	// RLerp와 TimeLine Value 값을 통한 자연스러운 기울이기
	const FRotator RLerp = UKismetMathLibrary::RLerp(CameraCurrentRotation, CameraDesiredRotation, Value, true);
	const FVector VLerp = UKismetMathLibrary::VLerp(CameraCurrentPosition, CameraDesiredPosition, Value);
	const FTransform TLerp = UKismetMathLibrary::MakeTransform(VLerp, RLerp);
	// 해당 트랜스폼 할당
	FirstPersonCamera->SetRelativeTransform(TLerp);
	
	// 애님에게 값 전달.
	const float TiltValue = UKismetMathLibrary::Lerp(CurrentTiltAngle, DesiredTiltAngle,Value);
	TiltAngle = TiltValue;
	ServerSetTiltAngle(TiltAngle);

	const FVector TiltLocationValue = UKismetMathLibrary::VLerp(CurrentTiltLocation, DesiredTiltLocation,Value);
	TiltLocation = TiltLocationValue;
	ServerSetTiltLocation(TiltLocation);
}

void ACHCharacterPlayer::TiltRight()
{
	if (bIsFirstPersonPerspective)
	{
		// UE_LOG(LogTemp, Log, TEXT("ACHCharacterPlayer::TiltRight()"));
		bTiltReleaseLeft = false;
		bTiltReleaseRight = false;
		if (TiltingLeftTimeline.IsPlaying())
		{
			TiltingLeftTimeline.Stop();
		}
		if (TiltingRightTimeline.IsPlaying())
		{
			TiltingRightTimeline.Stop();
		}
		TiltingRightTimeline.PlayFromStart();
	}

	// 3인칭 몸체 이동
	// 애니메이션도 타임라인으로 어떻게 안되나??
	// TiltAngle		
		
	
}

void ACHCharacterPlayer::TiltRightRelease()
{
	if (bIsFirstPersonPerspective)
	{
		bTiltReleaseLeft = true;
		bTiltReleaseRight = true;
		if (TiltingLeftTimeline.IsPlaying())
		{
			TiltingLeftTimeline.Stop();
		}
		if (TiltingRightTimeline.IsPlaying())
		{
			TiltingRightTimeline.Stop();
		}
		TiltingRightTimeline.PlayFromStart();
	}
}

void ACHCharacterPlayer::TiltLeft()
{
	if (bIsFirstPersonPerspective)
	{
		// UE_LOG(LogTemp, Log, TEXT("ACHCharacterPlayer::TiltLeft()"));
		bTiltReleaseLeft = false;
		bTiltReleaseRight = false;
		if (TiltingLeftTimeline.IsPlaying())
		{
			TiltingLeftTimeline.Stop();
		}
		if (TiltingRightTimeline.IsPlaying())
		{
			TiltingRightTimeline.Stop();
		}
		TiltingLeftTimeline.PlayFromStart();
	}
	/*if(IsInFirstPersonPerspective())
	{
		UE_LOG(LogTemp, Log, TEXT("ACHCharacterPlayer::LeftTilt()"));
		TiltAngle = 70;
		// Camera1PBoom->SetWorldRotation(FRotator(0,TiltAngle,0));
		// Camera1PBoom->SetRelativeRotation(FRotator(Camera1PBoom->GetComponentRotation().Pitch,Camera1PBoom->GetComponentRotation().Yaw, Camera1PBoom->GetComponentRotation().Roll - 20));
	}*/
}

void ACHCharacterPlayer::TiltLeftRelease()
{
	if (bIsFirstPersonPerspective)
	{
		bTiltReleaseLeft = true;
		bTiltReleaseRight = true;
		if (TiltingLeftTimeline.IsPlaying())
		{
			TiltingLeftTimeline.Stop();
		}
		if (TiltingRightTimeline.IsPlaying())
		{
			TiltingRightTimeline.Stop();
		}
		TiltingLeftTimeline.PlayFromStart();
	}
}

void ACHCharacterPlayer::PressV()
{
	TogglePerspective();
	ServerRPC_SetPerspective(bIsFirstPersonPerspective);
}

void ACHCharacterPlayer::TogglePerspective()
{
	if (CurrentCharacterControlType == ECharacterControlType::ThirdAim
		|| CurrentCharacterControlType == ECharacterControlType::ThirdPrecisionAim
		|| CurrentCharacterControlType == ECharacterControlType::FirstAim
		|| CurrentCharacterControlType == ECharacterControlType::FirstScopeAim) return;	
	// 서버 RPC 날리기
	bIsFirstPersonPerspective = !bIsFirstPersonPerspective;
	SetPerspective(bIsFirstPersonPerspective);
}

void ACHCharacterPlayer::ServerRPC_SetPerspective_Implementation(uint8 Is1PPerspective)
{
	TogglePerspective();
	// bIsFirstPersonPerspective = Is1PPerspective;
	// OnRep_FirstPersonPerspective();
	// MulticastRPC_SetPerspective(bIsFirstPersonPerspective);	
}

bool ACHCharacterPlayer::ServerRPC_SetPerspective_Validate(uint8 Is1PPerspective)
{
	return true;
}

/*void ACHCharacterPlayer::MulticastRPC_SetPerspective_Implementation(bool Is1PPerspective)
{
	// 클라 && 서버 X => 클라 본인 제외 
	// if(IsLocallyControlled() && !HasAuthority()) return;
	SetPerspective(Is1PPerspective);
}*/

void ACHCharacterPlayer::SetPerspective(uint8 Is1PPerspective)
{
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	
	if (Is1PPerspective)
	{
		// 1인칭
		SetCharacterControl(ECharacterControlType::First);
		// ServerRPC_SetCharacterControl(ECharacterControlType::First);
		
		StopCover();
		bCovered = false;

		// 이하 동기화 X
		if(IsLocallyControlled())
		{
			ThirdPersonCamera->Deactivate();
			GetMesh()->SetVisibility(false, true);
			GetMesh()->CastShadow = true;
			GetMesh()->bCastHiddenShadow = true;

			FirstPersonCamera->Activate();
			FirstPersonMesh->SetVisibility(true);
			if(CurrentWeapon)
			{
				UE_LOG(LogTemp, Log, TEXT("CurrentWeapon : %d"), CurrentWeapon->WeaponType);
				CurrentWeapon->GetWeaponMesh1P()->SetVisibility(true, true);
				CurrentWeapon->SetWeaponMeshVisibility(false);
			}
		}		
	}
	else
	{
		// 3인칭
		bCovered = false;
		SetCharacterControl(ECharacterControlType::Third);
		// ServerRPC_SetCharacterControl(ECharacterControlType::Third);
		
		FirstPersonCamera->Deactivate();
		FirstPersonMesh->SetVisibility(false, true);
		CH_LOG(LogCHNetwork, Log, TEXT("FirstPersonCamera->Deactivate();"))
		
		ThirdPersonCamera->Activate();
		GetMesh()->SetVisibility(true);
		GetMesh()->CastShadow = true;
		GetMesh()->bCastHiddenShadow = true;
		
		if(CurrentWeapon)
		{
			UE_LOG(LogTemp, Log, TEXT("CurrentWeapon : %d"), CurrentWeapon->WeaponType);
			CurrentWeapon->GetWeaponMesh3P()->SetVisibility(true, true);
		}
	}
	CH_LOG(LogCHNetwork, Log, TEXT("End"))
}

void ACHCharacterPlayer::OnRep_FirstPersonPerspective()
{
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	if(CurrentWeapon)
	{
		if(bIsFirstPersonPerspective)
		{
			CurrentWeapon->MuzzleCollision1P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			CurrentWeapon->MuzzleCollision3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);			
		}
		else
		{
			CurrentWeapon->MuzzleCollision1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CurrentWeapon->MuzzleCollision3P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
	
	
	
	CH_LOG(LogCHNetwork, Log, TEXT("End"))
}

void ACHCharacterPlayer::SetCoveredAttackMotion(uint8 bAim)
{
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	switch (CHAnimInstance->GetCurrentCoverState())
	{
	case ECoverState::Low:
		if(bAim)
		{
			UnCrouch();
			CH_LOG(LogCHNetwork, Log, TEXT("UnCrouch"))
		}
		else
		{
			Crouch();
			ReturnCover();
			CH_LOG(LogCHNetwork, Log, TEXT("Crouch"))
		}
		break;
	case ECoverState::High:
		// 벽 중간에서 aim하면 이동???
		if(bAim)
		{
			UCHCharacterControlData* NewCharacterControl = CharacterControlManager[ECharacterControlType::ThirdCover];
			CameraBoom->SocketOffset = FVector(NewCharacterControl->SocketOffset.X,NewCharacterControl->SocketOffset.Y * InputVectorDirectionByCamera,NewCharacterControl->SocketOffset.Z);
			float Radius = GetCapsuleComponent()->GetScaledCapsuleRadius();
			CH_LOG(LogCHNetwork, Log, TEXT("MoveDirection : %s Radius : %f"), *MoveDirection.ToString(), Radius)
			// AddMovementInput(GetActorLocation() + MoveDirection * Radius * 2);
			SetActorLocation(GetActorLocation() + MoveDirection * Radius * 2);
			// 도는 방향 정하기
			CH_LOG(LogCHNetwork, Log, TEXT("Aimed Location : %s"), *GetActorLocation().ToString())			
		}
		else
		{
			UCHCharacterControlData* NewCharacterControl = CharacterControlManager[ECharacterControlType::ThirdCover];
			CameraBoom->SocketOffset = FVector(NewCharacterControl->SocketOffset.X,NewCharacterControl->SocketOffset.Y * InputVectorDirectionByCamera,NewCharacterControl->SocketOffset.Z);
			float Radius = GetCapsuleComponent()->GetScaledCapsuleRadius();
			// CH_LOG(LogCHNetwork, Log, TEXT("LastCoveredRotation : %s "), *LastCoveredRotation.ToString())
			// AddMovementInput(GetActorLocation() - MoveDirection * GetCapsuleComponent()->GetScaledCapsuleRadius() * 2);
			SetActorLocation(GetActorLocation() - MoveDirection * GetCapsuleComponent()->GetScaledCapsuleRadius() * 2);
			
			CH_LOG(LogCHTemp, Log, TEXT("LastCoveredRotation : %s"), *LastCoveredRotation.ToString())
			CH_LOG(LogCHTemp, Log, TEXT("Before ActorRotation : %s"), *GetActorRotation().ToString())
			SetActorRotation(LastCoveredRotation);
			CH_LOG(LogCHTemp, Log, TEXT("After ActorRotation : %s"), *GetActorRotation().ToString())

			
			CH_LOG(LogCHNetwork, Log, TEXT("UnAimed Location : %s"), *GetActorLocation().ToString())
		}
		break;
	case ECoverState::None:
		break;
	}
	CH_LOG(LogCHNetwork, Log, TEXT("End"))		
}

void ACHCharacterPlayer::ServerSetActorRotation_Implementation(FRotator NewRotator)
{
	LastCoveredRotation = NewRotator;
	SetActorRotation(LastCoveredRotation);
}

bool ACHCharacterPlayer::ServerSetActorRotation_Validate(FRotator NewRotator)
{
	return true;
}

void ACHCharacterPlayer::ServerSetMoveDirection_Implementation(FVector NewMoveDirection)
{
	MoveDirection = NewMoveDirection;
}

bool ACHCharacterPlayer::ServerSetMoveDirection_Validate(FVector NewMoveDirection)
{
	return true;
}

void ACHCharacterPlayer::MulticastSetCoveredAttackMotion_Implementation(uint8 bAim)
{
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	SetCoveredAttackMotion(bAim);
	CH_LOG(LogCHNetwork, Log, TEXT("End"))
}

void ACHCharacterPlayer::ServerSetCoveredAttackMotion_Implementation(uint8 bAim)
{
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	// SetCoveredAttackMotion(bAim);
	MulticastSetCoveredAttackMotion(bAim);
	CH_LOG(LogCHNetwork, Log, TEXT("End"))
}

bool ACHCharacterPlayer::ServerSetCoveredAttackMotion_Validate(uint8 bAim)
{
	return true;
}

void ACHCharacterPlayer::PressSprint()
{
	StartSprint();
	ServerRPC_StartSprint();
}

void ACHCharacterPlayer::ReleaseSprint()
{
	StopSprint();	// 로컬
	ServerRPC_StopSprint();	// 서버 바꾸기
}

void ACHCharacterPlayer::StartSprint()
{
	Super::StartSprint();
}

void ACHCharacterPlayer::StopSprint()
{
	Super::StopSprint();
}

void ACHCharacterPlayer::SetupHUDWidget(UCHHUDWidget* InHUDWidget)
{
	if (InHUDWidget)
	{
		InHUDWidget->SetMaxHp_Test(Stat->GetMaxHp());
		InHUDWidget->UpdateHpBar(Stat->GetCurrentHp());

		// Stat->OnStatChanged.AddUObject(InHUDWidget, &UCHHUDWidget::UpdateStat);
		Stat->OnHpChanged.AddUObject(InHUDWidget, &UCHHUDWidget::UpdateHpBar);
		OnCombat.AddUObject(InHUDWidget, &UCHHUDWidget::SetCombatMode);
	}
}

void ACHCharacterPlayer::SetupCrossWidget(UCHUserWidget* InUserWidget)
{
	UUCHCrossHairWidget* CrossWidget = Cast<UUCHCrossHairWidget>(InUserWidget);
	if (CrossWidget)
	{
		// OnCombat.AddUObject(CrossWidget, &UUCHCrossHairWidget::SetCombatMode);
	}
}

