#pragma once

#include "GameFramework/Actor.h"
#include "TC_FishPondTypes.h"

#include "TC_FishingAreaVolume.generated.h"

UCLASS()
class FISHPOND_API ATC_FishingAreaVolume : public AActor
{
	GENERATED_BODY()

public:
	ATC_FishingAreaVolume();

	bool ContainsLocation(const FVector& WorldLocation) const;

private:
	UFUNCTION()
	void HandleAreaBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fishing Area", meta=(AllowPrivateAccess=true))
	class UBoxComponent* AreaBounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing Area|Spawning", meta=(AllowPrivateAccess=true))
	TArray<FTC_FishingAreaSpawnConfig> AreaSpawnConfigs;
};
