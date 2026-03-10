#pragma once

#include "Components/ActorComponent.h"

#include "TC_FishingFSMComponent.generated.h"

UENUM(BlueprintType)
enum class ETC_FishingState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Casting UMETA(DisplayName = "Casting"),
	WaitingForBite UMETA(DisplayName = "Waiting For Bite"),
	ReelingIn UMETA(DisplayName = "Reeling In"),
	Caught UMETA(DisplayName = "Caught")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FISHERMEN_API UTC_FishingFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTC_FishingFSMComponent();

	UFUNCTION(BlueprintCallable, Category = "Fishing|FSM")
	void SetState(ETC_FishingState NewState);

	UFUNCTION(BlueprintPure, Category = "Fishing|FSM")
	ETC_FishingState GetState() const;

	UFUNCTION(BlueprintPure, Category = "Fishing|FSM")
	bool IsInState(ETC_FishingState InState) const;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fishing|FSM", meta=(AllowPrivateAccess=true))
	ETC_FishingState CurrentState;
};
