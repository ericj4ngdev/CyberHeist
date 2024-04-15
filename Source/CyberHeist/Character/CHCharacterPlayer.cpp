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
		ChangePerspectiveControlAction = InputChangeActionControlRef.Object;
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

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionThirdCoveredMoveActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_ThirdCoverdMoveAction.IA_ThirdCoverdMoveAction'"));
	if (nullptr != InputActionThirdCoveredMoveActionRef.Object)
	{
		ThirdCoveredMoveAction = InputActionThirdCoveredMoveActionRef.Object;
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
	// AngleForEscapeCover = 135.f;
	CurrentCharacterControlType = ECharacterControlType::Third;
}

void ACHCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	RadianForEscapeCover = FMath::DegreesToRadians(AngleForEscapeCover);
	UE_LOG(LogTemp, Log, TEXT("Cos(%f) : %f"), AngleForEscapeCover,FMath::Cos(AngleForEscapeCover));
	UE_LOG(LogTemp, Log, TEXT("Cos(%f) : %f"), RadianForEscapeCover,FMath::Cos(RadianForEscapeCover));
	CHAnimInstance = Cast<UCHAnimInstance>(GetMesh()->GetAnimInstance());
	
	SetCharacterControl(CurrentCharacterControlType);
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

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangePerspectiveControlAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::ChangePerspectiveControlData);
	
	EnhancedInputComponent->BindAction(FirstMoveAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::FirstMove);
	EnhancedInputComponent->BindAction(FirstLookAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::FirstLook);
	
	EnhancedInputComponent->BindAction(ThirdMoveAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::ThirdMove);
	EnhancedInputComponent->BindAction(ThirdCoveredMoveAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::ThirdCoveredMove);
	EnhancedInputComponent->BindAction(ThirdLookAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::ThirdLook);

	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACHCharacterPlayer::StartSprint);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACHCharacterPlayer::StopSprint);

	EnhancedInputComponent->BindAction(ChangeNextWeaponAction, ETriggerEvent::Triggered, this, &ACHCharacterBase::NextWeapon);
	EnhancedInputComponent->BindAction(ChangePrevWeaponAction, ETriggerEvent::Triggered, this, &ACHCharacterBase::PreviousWeapon);

	EnhancedInputComponent->BindAction(TakeCoverAction, ETriggerEvent::Triggered, this, &ACHCharacterPlayer::TakeCover);
}

void ACHCharacterPlayer::ChangePerspectiveControlData()
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
	CurrentCharacterControlType = NewCharacterControlType;

	FString EnumAsString = UEnum::GetValueAsString<ECharacterControlType>(CurrentCharacterControlType);
	UE_LOG(LogTemp, Log, TEXT("CurrentCharacterControlType : %s"), *EnumAsString);

	// UE_LOG(LogTemp, Log, TEXT("CurrentCharacterControlType : %s"), CurrentCharacterControlType)
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
	
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);				// (1,0,0)
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);					// (0,1,0)
	
	float speed = bSprint ? RunSpeed : WalkSpeed;
	
	// DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(), GetActorLocation() + ForwardDirection * 100.0f, 10.0f, FColor::Cyan, false, -1, 0 ,5.0f);
	// DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(), GetActorLocation() + RightDirection * 100.0f, 10.0f, FColor::Blue, false, -1, 0 ,5.0f);
	
	AddMovementInput(ForwardDirection, MovementVector.Y * speed);
	AddMovementInput(RightDirection, MovementVector.X * speed);
	GetCharacterMovement()->MaxWalkSpeed = speed;
	// UE_LOG(LogTemp, Log, TEXT("bSprint : %d	WalkSpeed : %f	RunSpeed : %f	speed : %f"), bSprint, WalkSpeed, RunSpeed, speed);
}

void ACHCharacterPlayer::ThirdLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);		// modify
}

void ACHCharacterPlayer::ThirdCoveredMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>().GetSafeNormal();
	
	if(bCovered)
	{
		const FVector BackwardVector = GetActorForwardVector();
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
		float InputVectorDirectionByCamera = 0.f;
		
		// case 1
		InputVectorDirectionByCamera = FVector::DotProduct(RightHand.GetSafeNormal(), InputVector.GetSafeNormal()) > 0 ? 1 : -1;

		// case 2
		/*InputVectorDirectionByCamera = FVector::DotProduct(RightHand.GetSafeNormal(), InputVector.GetSafeNormal());
		if(InputVectorDirectionByCamera > 0) InputVectorDirectionByCamera = 1;
		else if(InputVectorDirectionByCamera < 0) InputVectorDirectionByCamera = -1;*/
		
		UE_LOG(LogTemp, Log, TEXT("InputVectorDirectionByCamera : %f"), InputVectorDirectionByCamera);
		// 각도에 따라 위치가 달라진다... 그냥 크기 1,0,-1로 고정해야 할 듯.
		float Range = 150.0f;
		float Radius = 5.0f;
		FVector Start = RightHand * InputVectorDirectionByCamera * 45.0f  + GetActorLocation() + GetActorUpVector() * GetCapsuleComponent()->GetScaledCapsuleHalfHeight();			// 왼손, 오른손
		UE_LOG(LogTemp, Log, TEXT("GetCapsuleComponent()->GetScaledCapsuleHalfHeight() : %f"), GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		// FVector Start = RightHand * AngleForDirection * 45.0f  + GetActorLocation();			// 왼손, 오른손
		FVector End = Start + (BackwardVector * Range);											// 벽쪽으로 레이저. 근데 - 안붙혀도 뒤로 나간다. 
		
		FHitResult HitResult;
		FCollisionQueryParams TraceParams(FName(TEXT("CoverTrace")), true, this);
		bool HitDetected = GetWorld()->SweepSingleByChannel(HitResult, Start, End,FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(Radius), TraceParams);
		// WallNormal = HitResult.ImpactNormal;
		
#if ENABLE_DRAW_DEBUG
		FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
		DrawDebugCapsule(GetWorld(), CapsuleOrigin, Range * 0.5f, Radius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);		
#endif

		if(HitDetected == false) return;
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
			const FVector MoveDirection = WallParallel * AngleForDirection;
			
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
			SetCharacterControl(ECharacterControlType::Third);
			bCovered = false;
			UE_LOG(LogTemp, Log, TEXT("UnCovered"));
		}

		// 엄페 모서리에 도달 시 자동 해제... 는 아니고   
		// if(HitDetected == false)  
		
	}
}

void ACHCharacterPlayer::TakeCover()
{
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