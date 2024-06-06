// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon/Gun/CHGunBase.h"
#include "Interface/CHCharacterWidgetInterface.h"
#include "MotionWarpingComponent.h"
#include "CHCharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCombatModeSignature, uint8 /*combat */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHighCoverSignature, uint8 /*combat */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLowCoverSignature, uint8 /*combat */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCoverStateSignature, uint8 /*bHigh */, uint8 /*bLow */);

UENUM(BlueprintType)
enum class ECharacterControlType : uint8
{
	First,
	FirstAim,
	FirstScopeAim,
	Third,
	ThirdAim,
	ThirdCover,
	ThirdPrecisionAim,
};

USTRUCT(BlueprintType)
struct CYBERHEIST_API FCHCharacterInventory
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<ACHGunBase*> Weapons;
};


UCLASS()
class CYBERHEIST_API ACHCharacterBase : public ACharacter, public ICHCharacterWidgetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACHCharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	FOnCombatModeSignature OnCombat;
public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USceneComponent> SceneComponent;

	// Cover
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Cover")
	uint8 bCovered : 1;	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Cover")
	uint8 bHighCovered : 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Cover")
	uint8 bLowCovered : 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Cover")
	uint8 bCoverMoveRight : 1;
	
public:
	FOnHighCoverSignature OnHighCover;
	FOnLowCoverSignature OnLowCover;
	FOnCoverStateSignature OnCoverState;
	
	FORCEINLINE uint8 GetCovered() const { return bCovered;}
	FORCEINLINE uint8 GetHighCovered() const { return bHighCovered; }
	FORCEINLINE uint8 GetLowCovered() const { return bLowCovered; }
	FORCEINLINE uint8 GetCoverMoveRight() const { return bCoverMoveRight; }

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetCoverState(uint8 bNewHighCovered, uint8 bNewLowCovered);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetCoverMoveRight(uint8 bNewCoverMoveRight);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> TakeCoverMontage;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> FirstPersonMesh;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	USkeletalMeshComponent* GetFirstPersonMesh() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	USkeletalMeshComponent* GetThirdPersonMesh() const;
	
protected:
	virtual void SetCharacterControlData(const class UCHCharacterControlData* CharacterControlData);
	virtual void SetMappingContextPriority(const UInputMappingContext* MappingContext, int32 Priority);
	
	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UCHCharacterControlData*> CharacterControlManager;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	ECharacterControlType CurrentCharacterControlType;
	TMap<ECharacterControlType, class UCHCharacterControlData*> GetCharacterControlManager() {return CharacterControlManager;}
	FORCEINLINE ECharacterControlType GetCurrentCharacterControlType() { return CurrentCharacterControlType; }
	FORCEINLINE void SetCurrentCharacterControlType(ECharacterControlType Type) {	CurrentCharacterControlType = Type;	}

	// Aim
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Aiming, Category = Aim)
	uint8 bAiming : 1;
	UFUNCTION()
	void OnRep_Aiming();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_TPAimingCloser, Category = Aim)
	uint8 bTPAimingCloser : 1;
	UFUNCTION()
	void OnRep_TPAimingCloser();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_FPScopeAiming,Category = Aim)
	uint8 bFPScopeAiming : 1;
	UFUNCTION()
	void OnRep_FPScopeAiming();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Aim)
	float TiltAngle;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetTiltAngle(float NewTiltAngle);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Aim)
	FVector TiltLocation;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetTiltLocation(FVector NewTiltLocation);
	
	float CurrentTiltAngle;
	float DesiredTiltAngle;
	FVector CurrentTiltLocation;
	FVector DesiredTiltLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AimActionMontage;

public:
	void SetAiming(uint8 bNewAiming);
	
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);	
	FORCEINLINE uint8 GetAiming() const { return bAiming; }
	
	void SetTPAimingCloser(uint8 bNewTPAimingCloser);
	UFUNCTION(Server, Reliable)
	void ServerSetTPAimingCloser(bool bNewTPAimingCloser);	
	FORCEINLINE uint8 GetTPAimingCloser() const { return bTPAimingCloser; }
	
	FORCEINLINE void SetScopeAiming(uint8 bNewFPScopeAiming);
	UFUNCTION(Server, Reliable)
	void ServerSetFPScopeAiming(bool bNewFPScopeAiming);	
	FORCEINLINE uint8 GetScopeAiming() const { return bFPScopeAiming; }

	FORCEINLINE float GetTiltAngle() const { return TiltAngle; }
	FORCEINLINE FVector GetTiltLocation() const { return TiltLocation; }

	
public:
	virtual void SetCharacterControl(ECharacterControlType NewCharacterControlType);
	
	// Dead
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> DeadMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	uint8 bIsDead;
	
	// 죽는 함수
	virtual void SetDead();

	// 몽타주 재생 함수
	void PlayDeadAnimation();

	float DeadEventDelayTime = 5.0f;

	// Attack
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsAttacking;

	
public:
	uint8 GetIsDead();
	virtual void SetIsAttacking(uint8 IsAttack);
	uint8 GetIsAttacking();
	
public:	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void AttackHitCheck();
	
	FName GetWeaponAttachPoint() const;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Weapon)
	FName WeaponAttachPoint;

	UPROPERTY(BlueprintReadOnly, Category = Camera)
	FVector StartingThirdPersonMeshLocation;

	UPROPERTY(BlueprintReadOnly, Category = Camera)
	FVector StartingFirstPersonMeshLocation;
	
	
public:
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ACHGunBase> Weapon;*/

	// Walk speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed;

	// Run speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed;

	// Sneak Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SneakSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Sprint, Category = "Movement")
	uint8 bSprint : 1;

	virtual void StartSprint();
	virtual void StopSprint();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_StartSprint();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_StopSprint();
	
	/*UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Sprint(uint8 NewSprint);*/
	
	UFUNCTION()
	void OnRep_Sprint();

	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Motion, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMotionWarpingComponent> MotionWarpComponent;

	
	// Stat Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCHCharacterStatComponent> Stat;


	//UI Widget Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCHWidgetComponent> HpBar;

	virtual void SetupCharacterWidget(UCHUserWidget* InUserWidget) override;
	
	// Inventory
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, ReplicatedUsing = OnRep_Inventory, Category = "Inventory")
	FCHCharacterInventory Inventory;

	UFUNCTION()
	void OnRep_Inventory();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory")
	TArray<TSubclassOf<ACHGunBase>> DefaultInventoryWeaponClasses;
	
	UPROPERTY(EditAnywhere, Category = "Inventory", Meta = (AllowPrivateAccess = "true"))
	TMap<FString, class ACHGunBase*> InventoryManager;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddWeaponToInventory(ACHGunBase* NewGun, bool bEquipWeapon);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetCurrentWeapon(ACHGunBase* NewWeapon, ACHGunBase* LastWeapon);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EquipWeapon();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UnEquipWeapon(ACHGunBase* WeaponToUnEquip);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	ACHGunBase* GetCurrentWeapon() const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void NextWeapon();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void PreviousWeapon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon,Category = "Inventory")
	TObjectPtr<class ACHGunBase> CurrentWeapon;

	UFUNCTION()
	void OnRep_CurrentWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCUnEquipWeapon(ACHGunBase* LastWeapon);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCUnEquipWeapon(ACHGunBase* LastWeapon);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCEquipWeapon(ACHGunBase* NewWeapon, ACHGunBase* LastWeapon);
	
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPCEquipWeapon(ACHGunBase* NewWeapon, ACHGunBase* LastWeapon);
	
	

	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	// state
	uint8 bHasRPGInputBindings : 1;
	uint8 bHasRifleInputBindings : 1;
	uint8 bHasMinigunInputBindings : 1;

public:
	uint8 GetbHasRPGInputBindings(){return bHasRPGInputBindings;}
	uint8 GetbHasRifleInputBindings(){return bHasRifleInputBindings;}
	uint8 GetbHasMinigunInputBindings(){return bHasMinigunInputBindings;}
	void SetbHasRPGInputBindings(uint8 NewBool){bHasRPGInputBindings = NewBool;}
	void SetbHasRifleInputBindings(uint8 NewBool){bHasRifleInputBindings = NewBool;}
	void SetbHasMinigunInputBindings(uint8 NewBool){bHasMinigunInputBindings = NewBool;}
	
public:	
	TObjectPtr<class UCHAnimInstance> CHAnimInstance;

	void MoveActorLocation(const FVector& Destination, float InterpSpeed);

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite)
	uint8 bNearWall : 1;

	FORCEINLINE void SetNearWall(uint8 bNewNearWall){ bNearWall = bNewNearWall; }
	FORCEINLINE uint8 GetNearWall() const { return bNearWall; }
	// AI
public:
	virtual void NotifyComboActionEnd();
};

