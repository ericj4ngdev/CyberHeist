// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon/Gun/CHGun.h"
#include "Interface/CHCharacterWidgetInterface.h"
#include "MotionWarpingComponent.h"
#include "CHCharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCombatModeSignature, uint8 /*combat */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHighCoverSignature, uint8 /*combat */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLowCoverSignature, uint8 /*combat */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCoverStateSignature, uint8 /*bHigh */, uint8 /*bLow */);

UENUM()
enum class ECharacterControlType : uint8
{
	First,
	Third,
	FirstAim,
	ThirdAim,
	ThirdCover,
	ThirdCoverAim,
};

USTRUCT()
struct CYBERHEIST_API FCHCharacterInventory
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<ACHGun*> Weapons;

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

	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UCHCharacterControlData*> CharacterControlManager;

public:
	ECharacterControlType CurrentCharacterControlType;
	FORCEINLINE ECharacterControlType GetCurrentCharacterControlType() { return CurrentCharacterControlType; }
	FORCEINLINE void SetCurrentCharacterControlType(ECharacterControlType Type) {	CurrentCharacterControlType = Type;	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	uint8 bCombatMode : 1;
	void SetCombatMode(uint8 bNewCombatMode);
	uint8 GetCombatMode() { return bCombatMode; }

	// Cover
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	uint8 bCovered : 1;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> TakeCoverMontage;
	
	float CurrentDistanceFromWall;
	
public:
	virtual void ChangePerspectiveControlData();
	virtual void SetCharacterControl(ECharacterControlType NewCharacterControlType);
	// Aim
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AimActionMontage;

	void Aim();
	// Dead
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> DeadMontage;

	// 죽는 함수
	virtual void SetDead();
	// 몽타주 재생 함수
	void PlayDeadAnimation();

	float DeadEventDelayTime = 5.0f;	
	
public:
	void SetHasRifle(bool bNewHasRifle);
	bool GetHasRifle();

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

public:	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void AttackHitCheck();
	
	FName GetWeaponAttachPoint();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Weapon)
	FName WeaponAttachPoint;
	
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool IsInFirstPersonPerspective() const;
	
	UPROPERTY(BlueprintReadOnly, Category = Camera)
	bool bIsFirstPersonPerspective;

	UPROPERTY(BlueprintReadOnly, Category = Camera)
	FVector StartingThirdPersonMeshLocation;

	UPROPERTY(BlueprintReadOnly, Category = Camera)
	FVector StartingFirstPersonMeshLocation;

public:
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ACHGun> Weapon;*/

	// Walk speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed;

	// Run speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed;

	// Sneak Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SneakSpeed;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	uint8 bSprint : 1;

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
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GASShooter|Inventory")
	TArray<TSubclassOf<ACHGun>> DefaultInventoryWeaponClasses;
	
	UPROPERTY(EditAnywhere, Category = "Inventory", Meta = (AllowPrivateAccess = "true"))
	TMap<FString, class ACHGun*> InventoryManager;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddWeaponToInventory(ACHGun* NewGun, bool bEquipWeapon);

	void SetCurrentWeapon(ACHGun* NewWeapon, ACHGun* LastWeapon);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EquipWeapon(ACHGun* NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UnEquipWeapon(ACHGun* WeaponToUnEquip);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	ACHGun* GetCurrentWeapon() const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void NextWeapon();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void PreviousWeapon();

protected:
	UPROPERTY()
	ACHGun* CurrentWeapon;

	// AI
public:
	virtual void NotifyComboActionEnd();
};

