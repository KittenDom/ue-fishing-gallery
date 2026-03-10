#pragma once

#include "GameFramework/Character.h"

#include "TC_FishermanBase.generated.h"

UCLASS()
class FISHERMEN_API ATC_FishermanBase : public ACharacter
{
	GENERATED_BODY()

public:
	ATC_FishermanBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta=(AllowPrivateAccess=true))
    class USpringArmComponent* SpringArmComponent;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta=(AllowPrivateAccess=true))
    class UCameraComponent* CameraComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fishing, meta=(AllowPrivateAccess=true))
    class UTC_FishingPole* FishingPoleComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fishing, meta=(AllowPrivateAccess=true))
    class UTC_FishingFSMComponent* FishingFSMComponent;
};
