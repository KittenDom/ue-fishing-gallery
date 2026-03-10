#include "TC_FishSpawnPlanner.h"

#include "TC_Fish.h"
#include "TC_FishSettings.h"

TSubclassOf<ATC_Fish> FTC_FishSpawnPlanner::ResolveFishClass(const UTC_FishSettings& Settings, TSubclassOf<ATC_Fish> RequestedClass) const
{
	if (RequestedClass)
	{
		return RequestedClass;
	}

	return Settings.DefaultFishClass.LoadSynchronous();
}

TArray<FTC_FishSpawnRequest> FTC_FishSpawnPlanner::BuildSpawnRequestsFromSettings(const UTC_FishSettings& Settings) const
{
	TArray<FTC_FishSpawnRequest> SpawnRequests;

	for (const FTC_FishSpawnConfig& Config : Settings.FishSpawnConfigs)
	{
		if (Config.SpawnCount <= 0)
		{
			continue;
		}

		TSubclassOf<ATC_Fish> FishClass = Config.FishClass.LoadSynchronous();
		if (!FishClass)
		{
			continue;
		}

		FTC_FishSpawnRequest& SpawnRequest = SpawnRequests.AddDefaulted_GetRef();
		SpawnRequest.FishClass = TSoftClassPtr<ATC_Fish>(Config.FishClass.ToSoftObjectPath());
		SpawnRequest.SpawnCount = Config.SpawnCount;
	}

	if (SpawnRequests.Num() > 0)
	{
		return SpawnRequests;
	}

	if (Settings.DefaultSpawnCount <= 0 || Settings.DefaultFishClass.IsNull())
	{
		return SpawnRequests;
	}

	FTC_FishSpawnRequest& DefaultRequest = SpawnRequests.AddDefaulted_GetRef();
	DefaultRequest.FishClass = Settings.DefaultFishClass;
	DefaultRequest.SpawnCount = Settings.DefaultSpawnCount;
	return SpawnRequests;
}

TArray<FTransform> FTC_FishSpawnPlanner::BuildSpawnTransforms(const FBox& AreaBounds, int32 SpawnCount) const
{
	TArray<FTransform> SpawnTransforms;

	for (int32 i = 0; i < SpawnCount; ++i)
	{
		const FVector SpawnLocation(
			FMath::FRandRange(AreaBounds.Min.X, AreaBounds.Max.X),
			FMath::FRandRange(AreaBounds.Min.Y, AreaBounds.Max.Y),
			FMath::FRandRange(AreaBounds.Min.Z, AreaBounds.Max.Z));
		SpawnTransforms.Emplace(FRotator::ZeroRotator, SpawnLocation);
	}

	return SpawnTransforms;
}
