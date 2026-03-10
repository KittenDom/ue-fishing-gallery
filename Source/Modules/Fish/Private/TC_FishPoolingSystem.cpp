#include "TC_FishPoolingSystem.h"

#include "Engine/World.h"
#include "TC_Fish.h"
#include "TC_FishAreaActivationService.h"
#include "TC_FishPoolService.h"
#include "TC_FishSpawnPlanner.h"
#include "TC_FishSettings.h"

UTC_FishPoolingSystem::~UTC_FishPoolingSystem()
{
	if (PoolService != nullptr)
	{
		PoolService->Shutdown();
		delete PoolService;
		PoolService = nullptr;
	}

	delete AreaActivationService;
	AreaActivationService = nullptr;

	delete SpawnPlanner;
	SpawnPlanner = nullptr;
}

void UTC_FishPoolingSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	EnsureServices();
}

void UTC_FishPoolingSystem::EnsureServices()
{
	if (PoolService == nullptr)
	{
		PoolService = new FTC_FishPoolService();
	}

	if (SpawnPlanner == nullptr)
	{
		SpawnPlanner = new FTC_FishSpawnPlanner();
	}

	if (AreaActivationService == nullptr)
	{
		AreaActivationService = new FTC_FishAreaActivationService();
	}
}

void UTC_FishPoolingSystem::Deinitialize()
{
	EnsureServices();

	if (PoolService != nullptr)
	{
		PoolService->Shutdown();
		delete PoolService;
		PoolService = nullptr;
	}

	delete AreaActivationService;
	AreaActivationService = nullptr;

	delete SpawnPlanner;
	SpawnPlanner = nullptr;
	Super::Deinitialize();
}

void UTC_FishPoolingSystem::RegisterFish(ATC_Fish* Fish)
{
	EnsureServices();

	if (PoolService == nullptr)
	{
		return;
	}

	PoolService->RegisterFish(Fish);
}

void UTC_FishPoolingSystem::UnregisterFish(ATC_Fish* Fish)
{
	EnsureServices();

	if (PoolService == nullptr)
	{
		return;
	}

	PoolService->UnregisterFish(Fish);
}

TArray<ATC_Fish*> UTC_FishPoolingSystem::GetFishInBox(const FBox& Bounds) const
{
	const_cast<UTC_FishPoolingSystem*>(this)->EnsureServices();
	if (PoolService == nullptr)
	{
		return {};
	}

	return PoolService->GetFishInBox(Bounds);
}

ATC_Fish* UTC_FishPoolingSystem::SpawnFishFromPool(const FTransform& SpawnTransform, TSubclassOf<ATC_Fish> FishClass)
{
	EnsureServices();

	UWorld* World = GetWorld();
	const UTC_FishSettings* Settings = GetSettings();
	if (PoolService == nullptr || SpawnPlanner == nullptr || !IsValid(World) || !IsValid(Settings))
	{
		return nullptr;
	}

	TSubclassOf<ATC_Fish> TargetClass = SpawnPlanner->ResolveFishClass(*Settings, FishClass);
	if (!TargetClass)
	{
		return nullptr;
	}

	return PoolService->AcquireFish(*World, *Settings, TargetClass, SpawnTransform);
}

void UTC_FishPoolingSystem::ReleaseFishToPool(ATC_Fish* Fish)
{
	EnsureServices();

	if (PoolService == nullptr)
	{
		return;
	}

	PoolService->ReleaseFish(Fish);
}

TArray<ATC_Fish*> UTC_FishPoolingSystem::SpawnFishFromSettings(const TArray<FTransform>& SpawnTransforms)
{
	EnsureServices();

	const UTC_FishSettings* Settings = GetSettings();
	if (PoolService == nullptr || SpawnPlanner == nullptr || !IsValid(Settings))
	{
		return {};
	}

	TArray<ATC_Fish*> SpawnedFish;
	const TArray<FTC_FishSpawnRequest> SpawnRequests = SpawnPlanner->BuildSpawnRequestsFromSettings(*Settings);

	int32 TransformIndex = 0;
	for (const FTC_FishSpawnRequest& SpawnRequest : SpawnRequests)
	{
		TSubclassOf<ATC_Fish> FishClass = SpawnRequest.FishClass.LoadSynchronous();
		if (!FishClass || SpawnRequest.SpawnCount <= 0)
		{
			continue;
		}
	
		for (int32 i = 0; i < SpawnRequest.SpawnCount && TransformIndex < SpawnTransforms.Num(); ++i)
		{
			ATC_Fish* Spawned = SpawnFishFromPool(SpawnTransforms[TransformIndex], FishClass);
			if (IsValid(Spawned))
			{
				SpawnedFish.Add(Spawned);
			}

			++TransformIndex;
		}
	}

	return SpawnedFish;
}

void UTC_FishPoolingSystem::EnsurePoolInitialized()
{
	EnsureServices();

	UWorld* World = GetWorld();
	const UTC_FishSettings* Settings = GetSettings();
	if (PoolService == nullptr || !IsValid(World) || !IsValid(Settings))
	{
		return;
	}

	PoolService->Initialize(*World, *Settings);
}

bool UTC_FishPoolingSystem::IsPoolInitialized() const
{
	const_cast<UTC_FishPoolingSystem*>(this)->EnsureServices();
	return PoolService != nullptr && PoolService->IsInitialized();
}

TArray<ATC_Fish*> UTC_FishPoolingSystem::SetupActiveFishForArea(const TArray<FTC_FishSpawnRequest>& SpawnRequests, const FBox& AreaBounds)
{
	EnsureServices();

	UWorld* World = GetWorld();
	const UTC_FishSettings* Settings = GetSettings();
	if (AreaActivationService == nullptr || PoolService == nullptr || SpawnPlanner == nullptr || !IsValid(World) || !IsValid(Settings))
	{
		return {};
	}

	return AreaActivationService->ActivateFishForArea(*World, *Settings, *PoolService, *SpawnPlanner, SpawnRequests, AreaBounds);
}

const UTC_FishSettings* UTC_FishPoolingSystem::GetSettings() const
{
	return GetDefault<UTC_FishSettings>();
}
