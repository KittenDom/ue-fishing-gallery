// Copyright @subajat1 2026.

#pragma once

#include "GameFramework/PlayerController.h"

#include "TC_PlayerController.generated.h"

class UInputMappingContext;
/**
 * 
 */
UCLASS()
class FISHERMEN_API ATC_PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;


	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;
};
