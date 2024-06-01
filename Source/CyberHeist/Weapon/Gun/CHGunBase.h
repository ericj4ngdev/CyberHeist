// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "CHGunBase.generated.h"


UENUM(BlueprintType)
enum class ECHFireMode : uint8
{
	None			UMETA(DisplayName = "None"),
	FullAuto		UMETA(DisplayName = "FullAuto"),
	SemiAuto		UMETA(DisplayName = "SemiAuto"),
	BurstFire		UMETA(DisplayName = "BurstFire")
};

UENUM(BlueprintType)
enum class ECHWeaponType : uint8
{
	UnArmed,
	Rifle,
	MiniGun,
	RPG,
};


class ACHCharacterBase;

UCLASS()
class CYBERHEIST_API ACHGunBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACHGunBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds ) override;
	
	// Pickup on touch
	// virtual void NotifyActorBeginOverlap(class AActor* Other) override;

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class ACHCharacterBase> OwningCharacter;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UCapsuleComponent> CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> WeaponMesh1P;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> WeaponMesh3P;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> ScopeCamera;

	
public:
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh1P() const {return WeaponMesh1P;}
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh3P() const {return WeaponMesh3P;}
	
	// Montage
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> Equip1PMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> Equip3PMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> Fire1PMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> ScopeFire1PMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> Fire3PMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> AimFire1PMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> Reload1PMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> Reload3PMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> WeaponMeshFireMontage;
	
	/* 총 자체 재장전 애니메이션*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> ReloadWeaponMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> Withhold1PMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> Withhold3PMontage;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimMontage* GetEquip1PMontage() const;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimMontage* GetEquip3PMontage() const;

	// Offset
protected:
	// Relative Location of weapon 1P Mesh when equipped
	UPROPERTY(EditAnywhere, Category = "CHGunBase")
	FVector WeaponMesh1PEquippedRelativeLocation;

	// Relative Location of weapon 3P Mesh when equipped
	UPROPERTY(EditAnywhere, Category = "CHGunBase")
	FVector WeaponMesh3PEquippedRelativeLocation;
	
	UPROPERTY(EditAnywhere, Category = "Properties|Effect")
	TObjectPtr<class UParticleSystem> MuzzleFlash;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UParticleSystem> TraceParticles;

	UPROPERTY(EditAnywhere, Category = "Properties|Effect")
	TObjectPtr<class UParticleSystem> ImpactEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties|Effect")
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties|Audio")
	TObjectPtr<class USoundBase> FireSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties|Audio")
	TObjectPtr<class USoundBase> HitSound;

	// 총신 길이(총구 - 손잡이)
	// 각 총기 BeginPlay에서 계산	
	float BarrelLength;
	// 구체 위치 처음엔 총구. 
	// 구체는 장착하고서 위치시키기
	// bEquipped로 구분
	// 카메라 바라보는 방향을 어캐 가져오지...
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UCapsuleComponent> MuzzleCollision1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UCapsuleComponent> MuzzleCollision3P;

	UPROPERTY()
	TObjectPtr<const class USkeletalMeshSocket> HandleSocket_1P;

	UPROPERTY()
	TObjectPtr<const class USkeletalMeshSocket> HandleSocket_3P;

	UFUNCTION()
	void OnNearWall(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnFarFromWall(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	// properties
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties|Weapon")
	ECHWeaponType WeaponType;	

	UPROPERTY(VisibleAnywhere, Category = "Properties|Weapon")
	ECHFireMode DefaultFireMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties|Weapon")
	ECHFireMode FireMode;
	
	UPROPERTY(EditAnywhere, Category = "Properties|Fire")
	float FireInterval;

	UPROPERTY(EditAnywhere, Category = "Properties|Fire")
	float ReloadInterval;
	
	UPROPERTY(EditAnywhere, Category = "Properties|Fire")
	float ShootingPreparationTime = 0.2f;
	
	UPROPERTY(EditAnywhere, Category = "Properties|Fire")
	float MaxRange;
	
	UPROPERTY(EditAnywhere, Category = "Properties|Damage")
	float Damage;

	UPROPERTY(EditAnywhere, Category = "Properties|Damage")
	float HeadShotDamage;

	UPROPERTY(EditAnywhere, Category = "Properties|Recoil")
	float RecoilYaw;

	UPROPERTY(EditAnywhere, Category = "Properties|Recoil")
	float RecoilPitch;

	UPROPERTY(EditAnywhere, Category = "Properties|Recoil")
	float AimedRecoilYaw;

	UPROPERTY(EditAnywhere, Category = "Properties|Recoil")
	float AimedRecoilPitch;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties|Weapon")
	FName AttachPoint1P;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties|Weapon")
	FName AttachPoint3P;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties|Weapon")
	FName ShoulderPoint1P;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties|Weapon")
	FName ShoulderPoint3P;
	
	// How much ammo in the clip the gun starts with
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties|Ammo")
	int32 CurrentAmmoInClip;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties|Ammo")
	int32 ClipSize;

	// Ammo except in clip
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties|Ammo")
	int32 CurrentAmmo;
	
	// maximum bullet capacity
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties|Ammo")
	int32 MaxAmmoCapacity;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties|Ammo")
	uint8 bInfiniteAmmo : 1;
	
	FTimerHandle DurationTimerHandle;	
	FTimerHandle FireTimerHandle;
	FTimerHandle ShootTimerHandle;
	FTimerHandle ReloadTimerHandle;

	// Input
public:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimAction;

	/** Precision Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> PrecisionAimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CancelPrecisionAimAction;

	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ReloadAction;

public:
	
	virtual void Equip();
	virtual void UnEquip();
	virtual void Fire();	
	virtual void LocalFire(const FVector& HitLocation,const FVector& TraceEnd);
	virtual void PullTriggerByAI(AActor* AttackTarget);
	virtual void FireByAI(AActor* AttackTarget);
	virtual void AutoFireByAI(AActor* AttackTarget);
	virtual void EndShoot();
	virtual void PullTrigger();
	virtual void CancelPullTrigger();

	virtual void SetWeaponMeshVisibility(uint8 bVisible);
	
	UFUNCTION(BlueprintCallable)
	virtual void StartAim();
	UFUNCTION(BlueprintCallable)
	virtual void StopAim();
	UFUNCTION(BlueprintCallable)
	virtual void StartPrecisionAim();
	UFUNCTION(BlueprintCallable)
	virtual void StopPrecisionAim();

	virtual void StayPrecisionAim();
	virtual void Reload();
	virtual void SetupWeaponInputComponent();
	virtual void SetOwningCharacter(ACHCharacterBase* InOwningCharacter);
	virtual void StopParticleSystem();

	// state
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, ReplicatedUsing = OnRep_Equipped, Category = "Properties|State")
	uint8 bIsEquipped : 1;

	UFUNCTION()
	void OnRep_Equipped();
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, ReplicatedUsing = OnRep_Trigger,Category = "Properties|State")
	uint8 bTrigger : 1;

	UFUNCTION()
	void OnRep_Trigger();
	
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties|State")
	uint8 bReloading : 1;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties|State")
	uint8 bInputBindingsSetup : 1;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties|State")
	uint8 bHoldGun : 1;
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnRep_Owner() override;

public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCFire(const FVector& HitLocation, const FVector& TraceEnd);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCFire(const FVector& HitLocation, const FVector& TraceEnd);
	
	/*UPROPERTY(ReplicatedUsing = OnRep_CanAttack)
	uint8 bCanAttack : 1;

	UFUNCTION()
	void OnRep_CanAttack();*/
};
