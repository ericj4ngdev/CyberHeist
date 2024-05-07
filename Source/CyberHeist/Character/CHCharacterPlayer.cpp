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
#include "UI/UCHCrossHairWidget.h"

ACHCharacterPlayer::ACHCharacterPlayer()
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
	
	CurrentCharacterControlType = ECharacterControlType::Third;
	bIsFirstPersonPerspective = false;
}

void ACHCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	RadianForEscapeCover = FMath::DegreesToRadians(AngleForEscapeCover);
	UE_LOG(LogTemp, Log, TEXT("Cos(%f) : %f"), AngleForEscapeCover,FMath::Cos(AngleForEscapeCover));
	UE_LOG(LogTemp, Log, TEXT("Cos(%f) : %f"), RadianForEscapeCover,FMath::Cos(RadianForEscapeCover));
	CHAnimInstance = Cast<UCHAnimInstance>(GetMesh()->GetAnimInstance());
	// InputVectorDirectionByCamera = 0.f;
	StartingThirdPersonMeshLocation = GetMesh()->GetRelativeLocation();
	StartingFirstPersonMeshLocation = FirstPersonMesh->GetRelativeLocation();
	SetCharacterControl(CurrentCharacterControlType);
	SetPerspective(bIsFirstPersonPerspective);
}

void ACHCharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);				// (1,0,0) 카메라 전방
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);					// (0,1,0) 
	
	DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(), GetActorLocation() + ForwardDirection * 100.0f, 10.0f, FColor::Cyan, false, -1, 0 ,5.0f);
	DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(), GetActorLocation() + RightDirection * 100.0f, 10.0f, FColor::Blue, false, -1, 0 ,5.0f);
	
	FVector Start = GetActorUpVector() * 10.0f + GetActorLocation();
	FVector End = GetActorForwardVector() * 50.0f + Start;
	DrawDebugDirectionalArrow(GetWorld(),Start, End, 10.0f, FColor::Red, false, -1, 0 ,10.0f);

	// GetActorRotation().Vector() = Forward()
}

void ACHCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangePerspectiveControlAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::TogglePerspective);
	// EnhancedInputComponent->BindAction(ChangePerspectiveControlAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::ChangePerspectiveControlData);

	
	EnhancedInputComponent->BindAction(FirstMoveAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::FirstMove);
	EnhancedInputComponent->BindAction(FirstLookAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::FirstLook);
	
	EnhancedInputComponent->BindAction(ThirdMoveAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::ThirdMove);
	// EnhancedInputComponent->BindAction(ThirdCoveredMoveAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::ThirdCoveredMove);
	EnhancedInputComponent->BindAction(ThirdLookAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::ThirdLook);

	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACHCharacterPlayer::StartSprint);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACHCharacterPlayer::StopSprint);

	EnhancedInputComponent->BindAction(ChangeNextWeaponAction, ETriggerEvent::Triggered, this, &ACHCharacterBase::NextWeapon);
	EnhancedInputComponent->BindAction(ChangePrevWeaponAction, ETriggerEvent::Triggered, this, &ACHCharacterBase::PreviousWeapon);

	EnhancedInputComponent->BindAction(TakeCoverAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::TakeCover);
	EnhancedInputComponent->BindAction(RightTiltAction, ETriggerEvent::Started, this, &ACHCharacterPlayer::RightTilt);
	EnhancedInputComponent->BindAction(LeftTiltAction, ETriggerEvent::Started, this, &ACHCharacterPlayer::LeftTilt);
	EnhancedInputComponent->BindAction(RightTiltAction, ETriggerEvent::Completed, this, &ACHCharacterPlayer::StopTilt);
	EnhancedInputComponent->BindAction(LeftTiltAction, ETriggerEvent::Completed, this, &ACHCharacterPlayer::StopTilt);
}

void ACHCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	UCHCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	UCHCharacterControlData* PrevCharacterControl = CharacterControlManager[CurrentCharacterControlType];

	// when player take covered in Fist personview and uncovered, new = Third exception
	if(PrevCharacterControl == CharacterControlManager[ECharacterControlType::First] && bCovered)
	{
		NewCharacterControl = CharacterControlManager[ECharacterControlType::First];
	}
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);		// ControlData(Camera and Pawn Rotation)

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());

	// Change IMC 
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{	
		UInputMappingContext* PrevMappingContext = PrevCharacterControl->InputMappingContext;
		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
		/*if(PrevMappingContext->GetName() != NewMappingContext->GetName())
		{*/
			if (PrevMappingContext)
			{
				Subsystem->RemoveMappingContext(PrevMappingContext);			
			}
		
			if (NewMappingContext)
			{
				Subsystem->AddMappingContext(NewMappingContext, 1);
			
			}
		UE_LOG(LogTemp, Log, TEXT("Changed %s to %s"), *PrevMappingContext->GetName(), *NewMappingContext->GetName());
		
		// IMC.Add(Subsystem->GetPlayerInput());
		// UE_LOG(LogTemp, Log, TEXT("%s"), *IMC[0]->GetName()); 
	}
	CurrentCharacterControlType = NewCharacterControlType;

	FString EnumAsString = UEnum::GetValueAsString<ECharacterControlType>(CurrentCharacterControlType);
	UE_LOG(LogTemp, Log, TEXT("CurrentCharacterControlType : %s"), *EnumAsString);

	// UE_LOG(LogTemp, Log, TEXT("CurrentCharacterControlType : %s"), CurrentCharacterControlType)
}

void ACHCharacterPlayer::SetCharacterControlData(const UCHCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	FirstPersonCamera->bUsePawnControlRotation = CharacterControlData->bFPP_UsePawnControlRotation;
	ThirdPersonCamera->bUsePawnControlRotation = CharacterControlData->bTPP_UsePawnControlRotation;
	
	// ThirdPersonCamera->SetRelativeLocation(CharacterControlData->CameraPosition);
	FirstPersonCamera->SetRelativeLocation(CharacterControlData->FP_CameraPosition);
	FirstPersonCamera->FieldOfView = CharacterControlData->FP_FieldOfView;
	ThirdPersonCamera->FieldOfView = CharacterControlData->TP_FieldOfView;
	
	CameraBoom->SocketOffset = CharacterControlData->TP_CameraPosition;	
	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
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
			UE_LOG(LogTemp, Log, TEXT("SetMappingContextPriority"));
			UE_LOG(LogTemp, Log, TEXT("Changed %s to %s"), *MappingContext->GetName(), *MappingContext->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("There is no MappingContext"));
		}
	}
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
	if(bAiming) speed = WalkSpeed;
	if(CurrentWeapon)
	{
		if(CurrentWeapon->WeaponType == ECHWeaponType::MiniGun) speed = WalkSpeed;
	}
	AddMovementInput(ForwardDirection, MovementVector.Y * speed);
	AddMovementInput(RightDirection, MovementVector.X * speed);

	GetCharacterMovement()->MaxWalkSpeed = speed;

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
		FVector Start = RightHand * InputVectorDirectionByCamera * 45.0f  + GetActorLocation() + GetActorUpVector() * GetCapsuleComponent()->GetScaledCapsuleHalfHeight();			// 왼손, 오른손
		// UE_LOG(LogTemp, Log, TEXT("GetCapsuleComponent()->GetScaledCapsuleHalfHeight() : %f"), GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		// FVector Start = RightHand * AngleForDirection * 45.0f  + GetActorLocation();			// 왼손, 오른손
		FVector End = Start + (ForwardVector * Range);											// 벽쪽으로 레이저. 근데 - 안붙혀도 뒤로 나간다. 
		
		FHitResult HitResult;
		FCollisionQueryParams TraceParams(FName(TEXT("CoverTrace")), true, this);
		bool HitDetected = GetWorld()->SweepSingleByChannel(HitResult, Start, End,FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(Radius), TraceParams);
		bEdge = HitDetected;
		// WallNormal = HitResult.ImpactNormal;
		
#if ENABLE_DRAW_DEBUG
		FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
		DrawDebugCapsule(GetWorld(), CapsuleOrigin, Range * 0.5f, Radius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);		
#endif

		if(HitDetected == false)
		{
			// 카메라 이동
			// AngleForDirection > 0     // 방향
			UCHCharacterControlData* NewCharacterControl = CharacterControlManager[ECharacterControlType::ThirdCover];
			CameraBoom->SocketOffset = FVector(NewCharacterControl->TP_CameraPosition.X,NewCharacterControl->TP_CameraPosition.Y * InputVectorDirectionByCamera,NewCharacterControl->TP_CameraPosition.Z);
			// UE_LOG(LogTemp, Log, TEXT("NewCharacterControl->CameraPosition.Y : %f"), NewCharacterControl->CameraPosition.Y);

				
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
		UE_LOG(LogTemp, Log, TEXT("AngleForDirection : %f "),AngleForDirection);

		// 40 degree ~ 180 degree = can cover
		// Degree Uproperty 
		if(AngleForCoveredMove >= -1.0f && AngleForCoveredMove <= FMath::Cos(RadianForEscapeCover))
		{
			CHAnimInstance->SetCoveredDirection(AngleForDirection > 0);
			// bool IsRight = (AngleForDirection > 0) ? true : false; 
			MoveDirection = WallParallel * AngleForDirection;
			
			SetActorRotation((-HitResult.ImpactNormal).Rotation());
			AddMovementInput(MoveDirection, SneakSpeed);
			// Want to = 법선 벡터 
			// current = 현재 내 액터의 rotation
		}
		else
		{
			// Cancel Cover Anim Montage
			OnCoverState.Broadcast(false, false);
			UnCrouch();
			// 입력 속성 변경

			// Aim이면 조준상태 유지하면서 나오게 하기 위함
			if(CurrentCharacterControlType == ECharacterControlType::ThirdCover)
			{
				SetCharacterControl(ECharacterControlType::Third);
			}			
			
			bCovered = false;
			UE_LOG(LogTemp, Log, TEXT("UnCovered"));
		}		
	}
	else
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);	

		float speed = bSprint ? RunSpeed : WalkSpeed;
		if(bAiming) speed = WalkSpeed;
		if(CurrentWeapon)
		{
			if(CurrentWeapon->WeaponType == ECHWeaponType::MiniGun) speed = WalkSpeed;
		}
	
		// DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(), GetActorLocation() + ForwardDirection * 100.0f, 10.0f, FColor::Cyan, false, -1, 0 ,5.0f);
		// DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(), GetActorLocation() + RightDirection * 100.0f, 10.0f, FColor::Blue, false, -1, 0 ,5.0f);
	
		AddMovementInput(ForwardDirection, MovementVector.Y * speed);
		AddMovementInput(RightDirection, MovementVector.X * speed);
		GetCharacterMovement()->MaxWalkSpeed = speed;
		// UE_LOG(LogTemp, Log, TEXT("bSprint : %d	WalkSpeed : %f	RunSpeed : %f	speed : %f"), bSprint, WalkSpeed, RunSpeed, speed);		
	}	
}

void ACHCharacterPlayer::ThirdLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);		// modify
}

void ACHCharacterPlayer::TakeCover()
{
	// 1인칭일 때는 비활성화
	if(IsInFirstPersonPerspective())
	{
		// 기울이기 기능 넣자 ㅋㅋㅋ
		return;
	}
	
	if(CurrentWeapon)
	{
		if(CurrentWeapon->WeaponType == ECHWeaponType::MiniGun) return;
	}
	
	// Not Covered 
	if(!bCovered)
	{
		float CharacterHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		FVector HighStart = GetActorLocation() + GetActorUpVector() * CharacterHalfHeight;				// 캐릭터 위치를 시작점으로 설정
		FVector LowerStart = GetActorLocation() - GetActorUpVector() * CharacterHalfHeight * 0.5f;		// 캐릭터 위치를 시작점으로 설정
		
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
			CHAnimInstance->SetCoveredDirection(Result > 0);			
			
			if(HitHighCoverDetected)
			{
				// Move to Cover
				FVector TargetLocation = HitLowCoverResult.Location;			
				FRotator TargetRotation = UKismetMathLibrary::NormalizedDeltaRotator(HitLowCoverResult.ImpactNormal.Rotation(), FRotator(0.0f, 180.0f,0.0f));
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
				}
				// 도착하면 모션 멈추기...				
				// Crouch();
				// 엄폐 애니메이션
				OnCoverState.Broadcast(true,true);
				
				// 움직임 속도 제한
				GetCharacterMovement()->MaxWalkSpeed = SneakSpeed;
				
				// 입력 속성 변경
				SetCharacterControl(ECharacterControlType::ThirdCover);
				
				UE_LOG(LogTemp, Log, TEXT("High Covered"));
			}
			else
			{
				// Move to Cover
				FVector TargetLocation = HitLowCoverResult.Location; //  + HitLowCoverResult.ImpactNormal * 1.0f;				
				FRotator TargetRotation = UKismetMathLibrary::NormalizedDeltaRotator(HitLowCoverResult.ImpactNormal.Rotation(), FRotator(0.0f, 180.0f,0.0f));
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
				}
				
				// 엄폐 애니메이션
				OnCoverState.Broadcast(false, true);
				Crouch();
				// 움직임 속도 제한
				GetCharacterMovement()->MaxWalkSpeed = SneakSpeed;
				
				// 입력 속성 변경
				// 1인칭일 때는 무시
				SetCharacterControl(ECharacterControlType::ThirdCover);
				
				UE_LOG(LogTemp, Log, TEXT("Low Covered"));
			}			
			bCovered = true;			
		}
		else
		{
			UnCrouch();
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
	else
	{
		UnCrouch();
		// Cancel Cover Anim Montage
		OnCoverState.Broadcast(false,false);
		// 입력 속성 변경
		SetCharacterControl(ECharacterControlType::Third);
			
		bCovered = false;
		UE_LOG(LogTemp, Log, TEXT("UnCovered"));
	}	
}

void ACHCharacterPlayer::RightTilt()
{
	if(IsInFirstPersonPerspective())
	{
		UE_LOG(LogTemp, Log, TEXT("ACHCharacterPlayer::RightTilt()"));
		TiltAngle = 110;
		// Camera1PBoom->SetWorldRotation(FRotator(Camera1PBoom->GetComponentRotation().Pitch,Camera1PBoom->GetComponentRotation().Yaw, TiltAngle));
		//Camera1PBoom->SetRelativeRotation(FRotator(Camera1PBoom->GetComponentRotation().Pitch,Camera1PBoom->GetComponentRotation().Yaw, TiltAngle));
		// Camera1PBoom->SetRelativeRotation(FRotator(Camera1PBoom->GetComponentRotation().Pitch,Camera1PBoom->GetComponentRotation().Yaw, Camera1PBoom->GetComponentRotation().Roll + 20));
	}
}

void ACHCharacterPlayer::LeftTilt()
{
	if(IsInFirstPersonPerspective())
	{
		UE_LOG(LogTemp, Log, TEXT("ACHCharacterPlayer::LeftTilt()"));
		TiltAngle = 70;
		// Camera1PBoom->SetWorldRotation(FRotator(0,TiltAngle,0));
		// Camera1PBoom->SetRelativeRotation(FRotator(Camera1PBoom->GetComponentRotation().Pitch,Camera1PBoom->GetComponentRotation().Yaw, Camera1PBoom->GetComponentRotation().Roll - 20));
	}
}

void ACHCharacterPlayer::StopTilt()
{
	if(IsInFirstPersonPerspective())
	{
		UE_LOG(LogTemp, Log, TEXT("ACHCharacterPlayer::StopTilt()"));
		TiltAngle = 90;		
		// Camera1PBoom->SetRelativeRotation(FRotator(Camera1PBoom->GetComponentRotation().Pitch,Camera1PBoom->GetComponentRotation().Yaw, TiltAngle));
	}
}

void ACHCharacterPlayer::TogglePerspective()
{
	bIsFirstPersonPerspective = !bIsFirstPersonPerspective;
	SetPerspective(bIsFirstPersonPerspective);
}

void ACHCharacterPlayer::SetPerspective(uint8 Is1PPerspective)
{
	if (CurrentCharacterControlType == ECharacterControlType::ThirdAim
	|| CurrentCharacterControlType == ECharacterControlType::ThirdPrecisionAim
	|| CurrentCharacterControlType == ECharacterControlType::FirstAim
	|| CurrentCharacterControlType == ECharacterControlType::FirstScopeAim) return;
	if (Is1PPerspective)
	{
		// 1인칭
		SetCharacterControl(ECharacterControlType::First);
		bCovered = false;
		
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
	else
	{
		bCovered = false;
		SetCharacterControl(ECharacterControlType::Third);
		
		FirstPersonCamera->Deactivate();
		FirstPersonMesh->SetVisibility(false, true);

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
}

void ACHCharacterPlayer::SetCoveredAttackMotion(uint8 bAim)
{
	switch (CHAnimInstance->GetCurrentCoverState())
	{
	case ECoverState::Low:
		if(bAim)
		{
			UnCrouch();			
		}
		else
		{
			Crouch();
		}
		break;
	case ECoverState::High:
		// 벽 중간에서 aim하면 이동???
		if(bAim)
		{
			UCHCharacterControlData* NewCharacterControl = CharacterControlManager[ECharacterControlType::ThirdCover];
			CameraBoom->SocketOffset = FVector(NewCharacterControl->TP_CameraPosition.X,NewCharacterControl->TP_CameraPosition.Y * InputVectorDirectionByCamera,NewCharacterControl->TP_CameraPosition.Z);
			SetActorLocation(GetActorLocation() + MoveDirection * GetCapsuleComponent()->GetScaledCapsuleRadius());
			UE_LOG(LogTemp, Log, TEXT("+ MoveDirection * GetCapsuleComponent()->GetScaledCapsuleRadius()"));				
		}
		else
		{
			UCHCharacterControlData* NewCharacterControl = CharacterControlManager[ECharacterControlType::ThirdCover];
			CameraBoom->SocketOffset = FVector(NewCharacterControl->TP_CameraPosition.X,NewCharacterControl->TP_CameraPosition.Y * InputVectorDirectionByCamera,NewCharacterControl->TP_CameraPosition.Z);
			SetActorLocation(GetActorLocation() - MoveDirection * GetCapsuleComponent()->GetScaledCapsuleRadius());
		}
		break;
	case ECoverState::None:
		break;
	}
		
}

void ACHCharacterPlayer::StartSprint() 
{	
	if(bAiming || GetCharacterMovement()->IsFalling())
	{
		bSprint = false;		
		return;
	}
	// if() return;
	UE_LOG(LogTemp, Log, TEXT("StartSprint"));
	bSprint = true;
}

void ACHCharacterPlayer::StopSprint()
{ 
	bSprint = false; 
	UE_LOG(LogTemp, Log, TEXT("bSprint is %s"), bSprint ? TEXT("true") : TEXT("false"));
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

