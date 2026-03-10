#include "TC_FishPoolService.h"

#include "TC_Fish.h"
#include "TC_FishSettings.h"

void FTC_FishPoolService::RegisterFish(ATC_Fish* Fish)
{
	if (!IsValid(Fish))
	{
		return;
	}

	RegisteredFish.AddUnique(Fish);
}

void FTC_FishPoolService::UnregisterFish(ATC_Fish* Fish)
{
	if (!IsValid(Fish))
	{
		return;
	}

	RegisteredFish.Remove(Fish);
}

TArray<ATC_Fish*> FTC_FishPoolService::GetFishInBox(const FBox& Bounds) const
{
	TArray<ATC_Fish*> FishInBox;

	for (ATC_Fish* Fish : RegisteredFish)
	{
		if (!IsValid(Fish))
		{
			continue;
		}

		if (Bounds.IsInsideOrOn(Fish->GetActorLocation()))
		{
			FishInBox.Add(Fish);
		}
	}

	return FishInBox;
}

bool FTC_FishPoolService::IsInitialized() const
{
	return bPoolInitialized;
}

void FTC_FishPoolService::Initialize(UWorld& World, const UTC_FishSettings& Settings)
{
	if (bPoolInitialized)
	{
		return;
	}

	for (const FTC_FishSpawnConfig& Config : Settings.FishSpawnConfigs)
	{
		TSubclassOf<ATC_Fish> FishClass = Config.FishClass.LoadSynchronous();
		if (!FishClass || Config.InitialPoolSize <= 0)
		{
			continue;
		}

		for (int32 i = 0; i < Config.InitialPoolSize && GetTotalPooledFishCount() < Settings.MaxPoolSize; ++i)
		{
			ATC_Fish* Fish = CreatePooledFish(World, FishClass, FTransform::Identity);
			if (!IsValid(Fish))
			{
				continue;
			}

			DeactivatePooledFish(Fish);
			InactivePoolByClass.FindOrAdd(FishClass).Add(Fish);
		}
	}

	bPoolInitialized = true;
}

ATC_Fish* FTC_FishPoolService::AcquireFish(UWorld& World, const UTC_FishSettings& Settings, TSubclassOf<ATC_Fish> FishClass, const FTransform& SpawnTransform)
{
	if (!bPoolInitialized)
	{
		Initialize(World, Settings);
	}

	if (!FishClass)
	{
		return nullptr;
	}

	TArray<TObjectPtr<ATC_Fish>>& InactivePool = InactivePoolByClass.FindOrAdd(FishClass);
	if (InactivePool.Num() > 0)
	{
		ATC_Fish* Fish = InactivePool.Pop(EAllowShrinking::No);
		if (!IsValid(Fish))
		{
			return nullptr;
		}

		ActivatePooledFish(Fish, SpawnTransform);
		ActivePooledFish.AddUnique(Fish);
		RegisterFish(Fish);
		return Fish;
	}

	if (GetTotalPooledFishCount() >= Settings.MaxPoolSize)
	{
		return nullptr;
	}

	ATC_Fish* Fish = CreatePooledFish(World, FishClass, SpawnTransform);
	if (!IsValid(Fish))
	{
		return nullptr;
	}

	ActivatePooledFish(Fish, SpawnTransform);
	ActivePooledFish.AddUnique(Fish);
	RegisterFish(Fish);
	return Fish;
}

void FTC_FishPoolService::ReleaseFish(ATC_Fish* Fish)
{
	if (!IsValid(Fish))
	{
		return;
	}

	const TSubclassOf<ATC_Fish> FishClass = Fish->GetClass();
	if (!FishClass)
	{
		return;
	}

	ActivePooledFish.Remove(Fish);
	UnregisterFish(Fish);
	DeactivatePooledFish(Fish);
	InactivePoolByClass.FindOrAdd(FishClass).AddUnique(Fish);
}

void FTC_FishPoolService::Shutdown()
{
	for (auto& PoolPair : InactivePoolByClass)
	{
		for (ATC_Fish* Fish : PoolPair.Value)
		{
			if (IsValid(Fish))
			{
				Fish->Destroy();
			}
		}
	}

	for (ATC_Fish* Fish : ActivePooledFish)
	{
		if (IsValid(Fish))
		{
			Fish->Destroy();
		}
	}

	RegisteredFish.Reset();
	InactivePoolByClass.Reset();
	ActivePooledFish.Reset();
	bPoolInitialized = false;
}

ATC_Fish* FTC_FishPoolService::CreatePooledFish(UWorld& World, TSubclassOf<ATC_Fish> FishClass, const FTransform& SpawnTransform)
{
	if (!FishClass)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	return World.SpawnActor<ATC_Fish>(FishClass, SpawnTransform, SpawnParams);
}

void FTC_FishPoolService::DeactivatePooledFish(ATC_Fish* Fish)
{
	if (!IsValid(Fish))
	{
		return;
	}

	Fish->SetActorHiddenInGame(true);
	Fish->SetActorEnableCollision(false);
	Fish->SetActorTickEnabled(false);
}

void FTC_FishPoolService::ActivatePooledFish(ATC_Fish* Fish, const FTransform& SpawnTransform)
{
	if (!IsValid(Fish))
	{
		return;
	}

	Fish->SetActorTransform(SpawnTransform);
	Fish->SetActorHiddenInGame(false);
	Fish->SetActorEnableCollision(true);
	Fish->SetActorTickEnabled(true);
}

int32 FTC_FishPoolService::GetTotalPooledFishCount() const
{
	int32 TotalCount = ActivePooledFish.Num();
	for (const auto& PoolPair : InactivePoolByClass)
	{
		TotalCount += PoolPair.Value.Num();
	}

	return TotalCount;
}
