#pragma once

#include "CoreMinimal.h"

#include "TC_FishSpawnTypes.generated.h"

USTRUCT(BlueprintType)
struct FISH_API FTC_FishSpawnRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSoftClassPtr<class ATC_Fish> FishClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning", meta=(ClampMin="0"))
	int32 SpawnCount = 0;
};
