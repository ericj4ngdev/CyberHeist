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

USTRUCT()
struct CYBERHEIST_API FCHCharacterInventory
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<ACHGunBase*> Weapons;

	// Consumable items

	// Passive items like armor

	// Door keys

	// Etc
};


UCLASS()
class CYBERHEIST_API ACHCharacterBase : public ACharacter, public ICHCharacterWidgetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACHCharacterBase();

	virtual void PostInitializeComponents() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FOnCombatModeSignature OnCombat;
public:
	FOnHighCoverSignature OnHighCover;
	FOnLowCoverSignature OnLowCover;
	FOnCoverStateSignature OnCoverState;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterControlType CurrentCharacterControlType;
	TMap<ECharacterControlType, class UCHCharacterControlData*> GetCharacterControlManager() {return CharacterControlManager;}
	FORCEINLINE ECharacterControlType GetCurrentCharacterControlType() { return CurrentCharacterControlType; }
	FORCEINLINE void SetCurrentCharacterControlType(ECharacterControlType Type) {	CurrentCharacterControlType = Type;	}

	// Aim
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Aim)
	uint8 bAiming : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Aim)
	uint8 bTPAimingCloser : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Aim)
	uint8 bFPScopeAiming : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Aim)
	float TiltAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AimActionMontage;

public:
	void Aim();
	
public:
	void SetAiming(uint8 bNewAiming);
	FORCEINLINE uint8 GetAiming() const { return bAiming; }
	FORCEINLINE void SetTPAimingCloser(uint8 bNewTPAimingCloser){ bTPAimingCloser = bNewTPAimingCloser; }
	FORCEINLINE uint8 GetTPAimingCloser() const { return bTPAimingCloser; }
	FORCEINLINE void SetScopeAiming(uint8 bNewFPScopeAiming){ bFPScopeAiming = bNewFPScopeAiming; }
	FORCEINLINE uint8 GetScopeAiming() const { return bFPScopeAiming; }
	FORCEINLINE float GetTiltAngle() const { return TiltAngle; }
	
	// Cover
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	uint8 bCovered : 1;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> TakeCoverMontage;

public:
	FORCEINLINE uint8 GetCovered() const { return bCovered;}
	// float CurrentDistanceFromWall;
	
public:
	virtual void SetCharacterControl(ECharacterControlType NewCharacterControlType);

	// Dead
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> DeadMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDead;
	
	// 죽는 함수
	virtual void SetDead();

	// 몽타주 재생 함수
	void PlayDeadAnimation();

	float DeadEventDelayTime = 5.0f;

	// Attack
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsAttacking;

	
public:
	bool GetIsDead();
	virtual void SetIsAttacking(uint8 IsAttack);
	uint8 GetIsAttacking();
	
public:	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void AttackHitCheck();
	
	FName GetWeaponAttachPoint() const;
	uint8 IsInFirstPersonPerspective() const;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Weapon)
	FName WeaponAttachPoint;
		
	UPROPERTY(BlueprintReadOnly, Category = Camera)
	uint8 bIsFirstPersonPerspective : 1;

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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	uint8 bSprint : 1;

	// UFUNCTION(Blueprintable)
	// void FootStepSound();

	
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
	UPROPERTY()
	FCHCharacterInventory Inventory;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory")
	TArray<TSubclassOf<ACHGunBase>> DefaultInventoryWeaponClasses;
	
	UPROPERTY(EditAnywhere, Category = "Inventory", Meta = (AllowPrivateAccess = "true"))
	TMap<FString, class ACHGunBase*> InventoryManager;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddWeaponToInventory(ACHGunBase* NewGun, bool bEquipWeapon);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetCurrentWeapon(ACHGunBase* NewWeapon, ACHGunBase* LastWeapon);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EquipWeapon(ACHGunBase* NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UnEquipWeapon(ACHGunBase* WeaponToUnEquip);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	ACHGunBase* GetCurrentWeapon() const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void NextWeapon();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void PreviousWeapon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<class ACHGunBase> CurrentWeapon;

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
	// AI
public:
	virtual void NotifyComboActionEnd();
};

