// Copyright @subajat1 2026.

#pragma once

#include "GameFramework/Actor.h"

#include "TC_FishingAreaTrigger.generated.h"

UCLASS()
class FISHPOND_API ATC_FishingAreaTrigger : public AActor
{
	GENERATED_BODY()

public:
	ATC_FishingAreaTrigger();

private:
	UFUNCTION()
	void HandleAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fishing Area", meta=(AllowPrivateAccess=true))
	class UBoxComponent* AreaBounds;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Fishing Area", meta=(AllowPrivateAccess=true))
	class ATC_FishingAreaVolume* fishingArea = nullptr;
};
