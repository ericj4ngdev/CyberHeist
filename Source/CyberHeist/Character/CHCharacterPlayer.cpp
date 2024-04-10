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
	// CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 0.0f);
	CameraBoom->bUsePawnControlRotation = true;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	// FirstPersonCamera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(),FName("Head"));	
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
	
	CurrentCharacterControlType = ECharacterControlType::Third;
}

void ACHCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	CHAnimInstance = Cast<UCHAnimInstance>(GetMesh()->GetAnimInstance());
	
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

	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACHCharacterPlayer::StartSprint);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACHCharacterPlayer::StopSprint);

	EnhancedInputComponent->BindAction(ChangeNextWeaponAction, ETriggerEvent::Triggered, this, &ACHCharacterBase::NextWeapon);
	EnhancedInputComponent->BindAction(ChangePrevWeaponAction, ETriggerEvent::Triggered, this, &ACHCharacterBase::PreviousWeapon);

	EnhancedInputComponent->BindAction(TakeCoverAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::TakeCover);
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
	UCHCharacterControlData* PrevCharacterControl = CharacterControlManager[CurrentCharacterControlType];
	
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		// Subsystem->ClearAllMappings();
		UInputMappingContext* PrevMappingContext = PrevCharacterControl->InputMappingContext;
		if (PrevMappingContext)
		{
			Subsystem->RemoveMappingContext(PrevMappingContext);			
		}
		
		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}
	CurrentCharacterControlType = NewCharacterControlType;   // ���� enum�� ��ü
}

void ACHCharacterPlayer::SetCharacterControlData(const UCHCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	// ThirdPersonCamera->SetRelativeLocation(CharacterControlData->CameraPosition);
	
	CameraBoom->SocketOffset = CharacterControlData->CameraPosition;
	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
}

void ACHCharacterPlayer::FirstMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>().GetSafeNormal();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	float speed = bSprint ? RunSpeed : WalkSpeed;

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
}

void ACHCharacterPlayer::ThirdMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>().GetSafeNormal();
	// UE_LOG(LogTemp, Log, TEXT("MovementVector X: %f, Y: %f"), MovementVector.X, MovementVector.Y);

	if(bCovered)
	{
		/*const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		UE_LOG(LogTemp, Log, TEXT("RightDirection X: %f, Y: %f | ForwardDirection X: %f, Y: %f"), RightDirection.X, RightDirection.Y, ForwardDirection.X, ForwardDirection.Y);*/
		
		const FRotator Rotation = GetActorRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		UE_LOG(LogTemp, Log, TEXT("RightDirection X: %f, Y: %f | ForwardDirection X: %f, Y: %f"), RightDirection.X, RightDirection.Y, ForwardDirection.X, ForwardDirection.Y);
		

		
		// FName HorizontalAxisName("Horizontal");
		// const FVector HorizontalVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); //InputComponent->GetAxisValue();

		/*const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		float speed = bSprint ? RunSpeed : WalkSpeed;

		AddMovementInput(ForwardDirection, MovementVector.Y * speed);
		AddMovementInput(RightDirection, MovementVector.X * speed);*/

		
		float Range = 100.0f;
		// GetMove Right/Left를 MovementVector로 하면 안될거 같다.
		// 진짜 회전값? yaw ?
		// 그런데 얘는 카메라 영향안받는데..?
		// 바로보는 방향의 좌우 (xy말고)
		FVector Start = RightDirection * 45.0f + GetActorLocation();
		FVector End = ForwardDirection * Range + Start;

		FHitResult HitResult;
		FCollisionQueryParams TraceParams(FName(TEXT("CoverTrace")), true, this);
		float Radius = 5.0f;
		bool HitDetected = GetWorld()->SweepSingleByChannel(HitResult, Start, End,
			FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(Radius), TraceParams);

#if ENABLE_DRAW_DEBUG
		FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
		DrawDebugCapsule(GetWorld(), CapsuleOrigin, Range * 0.5f, Radius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);		
#endif
		const FVector Temp = FRotationMatrix(FRotationMatrix::MakeFromX(HitResult.Normal).Rotator()).GetScaledAxis(EAxis::X);		// 벽과 평행한 좌우 방향
		AddMovementInput(Temp, - MovementVector.X * WalkSpeed);				
		// AddMovementInput(Temp, MovementVector.X * WalkSpeed);
	}
	else
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);		
		
		float speed = bSprint ? RunSpeed : WalkSpeed;

		AddMovementInput(ForwardDirection, MovementVector.Y * speed);
		AddMovementInput(RightDirection, MovementVector.X * speed);
	
		GetCharacterMovement()->MaxWalkSpeed = speed;
	}
	
	

	// UE_LOG(LogTemp, Log, TEXT("bSprint : %d	WalkSpeed : %f	RunSpeed : %f	speed : %f"), bSprint, WalkSpeed, RunSpeed, speed);
}

void ACHCharacterPlayer::ThirdLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);		// modify
}

void ACHCharacterPlayer::TakeCover()
{
	// Not Covered 
	if(!bCovered)
	{
		float CharacterHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		FVector HighStart = GetActorLocation() + GetActorUpVector() * CharacterHalfHeight;				// 캐릭터 위치를 시작점으로 설정
		FVector LowerStart = GetActorLocation() - GetActorUpVector() * CharacterHalfHeight * 0.5f;		// 캐릭터 위치를 시작점으로 설정
		
		float Radius = CharacterHalfHeight * 0.5f;									// 구체의 반경 설정
		float CheckRange = 150.0f;													// 엄폐물 조사 반경
		FVector HighEnd = HighStart + GetActorForwardVector() * CheckRange;			// 높이를 설정하여 바닥으로 Sphere Trace
		FVector LowerEnd = LowerStart + GetActorForwardVector() * CheckRange;		// 높이를 설정하여 바닥으로 Sphere Trace

		FHitResult HitHighCoverResult;
		FCollisionQueryParams HighTraceParams(FName(TEXT("HighCoverTrace")), true, this);

		FHitResult HitLowCoverResult;
		FCollisionQueryParams LowTraceParams(FName(TEXT("LowCoverTrace")), true, this);
		
		bool HitHighCoverDetected = GetWorld()->SweepSingleByChannel(HitHighCoverResult, HighStart, HighEnd,
			FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(Radius), HighTraceParams);
		bool HitLowCoverDetected = GetWorld()->SweepSingleByChannel(HitLowCoverResult, LowerStart, LowerEnd,
			FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(Radius), LowTraceParams);
		
		// 감지
		if (HitLowCoverDetected)
		{
			if(HitHighCoverDetected)
			{
				// Play Cover Anim Montage				
				FVector TargetLocation = HitHighCoverResult.Location + UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::MakeRotFromX(HitHighCoverResult.Normal)) * 15.0f;				
				FRotator TargetRotation = UKismetMathLibrary::NormalizedDeltaRotator(UKismetMathLibrary::MakeRotFromX(HitHighCoverResult.Normal), FRotator(0.0f,0.0f, 180.0f));
				// GetCapsuleComponent()->SetWorldLocationAndRotation(TargetLocation, TargetRotation);
				FLatentActionInfo Info;
				Info.CallbackTarget = this;
				UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(),TargetLocation, TargetRotation, false, false, 1.0f, false, EMoveComponentAction::Move, Info);
				OnLowCover.Broadcast(false);
				OnHighCover.Broadcast(true);
				// 움직임 속도 제한
				
				UE_LOG(LogTemp, Log, TEXT("High Covered"));
			}
			else
			{
				// Play Cover Anim Montage
				OnHighCover.Broadcast(false);
				OnLowCover.Broadcast(true);
				UE_LOG(LogTemp, Log, TEXT("Low Covered"));
			}			
			bCovered = true;			
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Nothing to Cover"));			
		}
#if ENABLE_DRAW_DEBUG

		FVector HigherCapsuleOrigin = HighStart + (HighEnd - HighStart) * 0.5f;
		FVector LowerCapsuleOrigin = LowerStart + (LowerEnd - LowerStart) * 0.5f;
		
		FColor DrawHighColor = HitHighCoverDetected ? FColor::Green : FColor::Red;		
		FColor DrawLowerColor = HitLowCoverDetected ? FColor::Green : FColor::Red;

		DrawDebugCapsule(GetWorld(), HigherCapsuleOrigin, CheckRange, Radius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawHighColor, false, 5.0f);
		DrawDebugCapsule(GetWorld(), LowerCapsuleOrigin, CheckRange, Radius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawLowerColor, false, 5.0f);
		
#endif
	}
	else
	{
		// Cancel Cover Anim Montage
		OnHighCover.Broadcast(false);
		OnLowCover.Broadcast(false);
		bCovered = false;
		UE_LOG(LogTemp, Log, TEXT("UnCovered"));
	}	
}

void ACHCharacterPlayer::StartSprint() 
{
	if(GetCharacterMovement()->IsFalling()) return;
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