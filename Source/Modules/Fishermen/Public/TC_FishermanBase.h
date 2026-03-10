#pragma once

#include "InputActionValue.h"
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
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;
	
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
