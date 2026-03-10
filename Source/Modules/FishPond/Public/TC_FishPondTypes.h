#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "TC_FishPondTypes.generated.h"

USTRUCT(BlueprintType)
struct FISHPOND_API FTC_FishingAreaSpawnConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing Area|Spawning")
	TSoftClassPtr<APawn> FishPawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing Area|Spawning", meta=(ClampMin="0"))
	int32 SpawnCount = 0;
};
