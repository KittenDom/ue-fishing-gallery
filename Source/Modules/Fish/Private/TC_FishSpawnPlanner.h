#pragma once

#include "CoreMinimal.h"
#include "TC_FishSpawnTypes.h"

class ITC_FishSpawnPlanner
{
public:
	virtual ~ITC_FishSpawnPlanner() = default;

	virtual TSubclassOf<class ATC_Fish> ResolveFishClass(const class UTC_FishSettings& Settings, TSubclassOf<class ATC_Fish> RequestedClass) const = 0;
	virtual TArray<FTC_FishSpawnRequest> BuildSpawnRequestsFromSettings(const class UTC_FishSettings& Settings) const = 0;
	virtual TArray<FTransform> BuildSpawnTransforms(const FBox& AreaBounds, int32 SpawnCount) const = 0;
};

class FTC_FishSpawnPlanner final : public ITC_FishSpawnPlanner
{
public:
	virtual TSubclassOf<class ATC_Fish> ResolveFishClass(const class UTC_FishSettings& Settings, TSubclassOf<class ATC_Fish> RequestedClass) const override;
	virtual TArray<FTC_FishSpawnRequest> BuildSpawnRequestsFromSettings(const class UTC_FishSettings& Settings) const override;
	virtual TArray<FTransform> BuildSpawnTransforms(const FBox& AreaBounds, int32 SpawnCount) const override;
};
