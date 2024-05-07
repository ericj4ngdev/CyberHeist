#pragma once

#define BBKEY_HOMEPOS TEXT("HomePos")
#define BBKEY_PATROLPOS TEXT("PatrolPos")
// #define BBKEY_TARGET TEXT("Target")
#define BBKEY_TARGETACTOR TEXT("TargetActor")
#define BBKEY_LASTKNOWNLOCATION TEXT("LastKnownLocation")
#define BBKEY_ISATTACKING TEXT("IsAttacking")


UENUM(BlueprintType)
enum class ECHAIState : uint8
{
	Passive			UMETA(DisplayName = "Passive"),				// 0
	Attacking		UMETA(DisplayName = "Attacking"),			// 1
	Frozen			UMETA(DisplayName = "Frozen"),				// 2
	Investigating	UMETA(DisplayName = "Investigating"),		// 3
	Dead			UMETA(DisplayName = "Dead"),				// 4
	Seeking			UMETA(DisplayName = "Seeking"),				// 5
};

UENUM(BlueprintType)
enum class ECHAIMovementSpeed : uint8
{
	Idle			UMETA(DisplayName = "Idle"),		// 0	
	Walking		UMETA(DisplayName = "Walking"),			// 1
	Jogging			UMETA(DisplayName = "Jogging"),		// 2
	Sprinting	UMETA(DisplayName = "Sprinting"),		// 3	
};

UENUM(BlueprintType)
enum class ECHAISense : uint8
{
	None			UMETA(DisplayName = "None"),		// 0	
	Sight		UMETA(DisplayName = "Sight"),			// 1
	Hearing			UMETA(DisplayName = "Hearing"),		// 2
	Damage	UMETA(DisplayName = "Damage"),		// 3	
};