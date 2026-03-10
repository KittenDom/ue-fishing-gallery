#pragma once

#include "CoreMinimal.h"
#include "TC_FishSpawnTypes.h"

class ATC_Fish;

class ITC_FishAreaActivationService
{
public:
	virtual ~ITC_FishAreaActivationService() = default;

	virtual TArray<ATC_Fish*> ActivateFishForArea(
		UWorld& World,
		const class UTC_FishSettings& Settings,
		class ITC_FishPoolService& PoolService,
		const class ITC_FishSpawnPlanner& SpawnPlanner,
		const TArray<FTC_FishSpawnRequest>& SpawnRequests,
		const FBox& AreaBounds) const = 0;
};

class FTC_FishAreaActivationService final : public ITC_FishAreaActivationService
{
public:
	virtual TArray<ATC_Fish*> ActivateFishForArea(
		UWorld& World,
		const class UTC_FishSettings& Settings,
		class ITC_FishPoolService& PoolService,
		const class ITC_FishSpawnPlanner& SpawnPlanner,
		const TArray<FTC_FishSpawnRequest>& SpawnRequests,
		const FBox& AreaBounds) const override;
};
