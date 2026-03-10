#include "TC_FishAreaActivationService.h"

#include "TC_Fish.h"
#include "TC_FishPoolService.h"
#include "TC_FishSettings.h"
#include "TC_FishSpawnPlanner.h"

TArray<ATC_Fish*> FTC_FishAreaActivationService::ActivateFishForArea(
	UWorld& World,
	const UTC_FishSettings& Settings,
	ITC_FishPoolService& PoolService,
	const ITC_FishSpawnPlanner& SpawnPlanner,
	const TArray<FTC_FishSpawnRequest>& SpawnRequests,
	const FBox& AreaBounds) const
{
	TArray<ATC_Fish*> ActivatedFish;

	if (!PoolService.IsInitialized())
	{
		PoolService.Initialize(World, Settings);
	}

	const TArray<ATC_Fish*> ExistingFishInArea = PoolService.GetFishInBox(AreaBounds);
	for (ATC_Fish* Fish : ExistingFishInArea)
	{
		PoolService.ReleaseFish(Fish);
	}

	for (const FTC_FishSpawnRequest& SpawnRequest : SpawnRequests)
	{
		TSubclassOf<ATC_Fish> FishClass = SpawnRequest.FishClass.LoadSynchronous();
		if (!FishClass || SpawnRequest.SpawnCount <= 0)
		{
			continue;
		}

		const TArray<FTransform> SpawnTransforms = SpawnPlanner.BuildSpawnTransforms(AreaBounds, SpawnRequest.SpawnCount);
		for (const FTransform& SpawnTransform : SpawnTransforms)
		{
			ATC_Fish* Fish = PoolService.AcquireFish(World, Settings, FishClass, SpawnTransform);
			if (!IsValid(Fish))
			{
				continue;
			}

			ActivatedFish.Add(Fish);
		}
	}

	return ActivatedFish;
}
