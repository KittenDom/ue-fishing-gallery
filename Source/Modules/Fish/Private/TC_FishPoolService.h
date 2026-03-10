#pragma once

#include "CoreMinimal.h"

class ATC_Fish;

class ITC_FishPoolService
{
public:
	virtual ~ITC_FishPoolService() = default;

	virtual void RegisterFish(ATC_Fish* Fish) = 0;
	virtual void UnregisterFish(ATC_Fish* Fish) = 0;
	virtual TArray<ATC_Fish*> GetFishInBox(const FBox& Bounds) const = 0;
	virtual bool IsInitialized() const = 0;
	virtual void Initialize(UWorld& World, const class UTC_FishSettings& Settings) = 0;
	virtual ATC_Fish* AcquireFish(UWorld& World, const class UTC_FishSettings& Settings, TSubclassOf<ATC_Fish> FishClass, const FTransform& SpawnTransform) = 0;
	virtual void ReleaseFish(ATC_Fish* Fish) = 0;
	virtual void Shutdown() = 0;
};

class FTC_FishPoolService final : public ITC_FishPoolService
{
public:
	virtual void RegisterFish(ATC_Fish* Fish) override;
	virtual void UnregisterFish(ATC_Fish* Fish) override;
	virtual TArray<ATC_Fish*> GetFishInBox(const FBox& Bounds) const override;
	virtual bool IsInitialized() const override;
	virtual void Initialize(UWorld& World, const class UTC_FishSettings& Settings) override;
	virtual ATC_Fish* AcquireFish(UWorld& World, const class UTC_FishSettings& Settings, TSubclassOf<ATC_Fish> FishClass, const FTransform& SpawnTransform) override;
	virtual void ReleaseFish(ATC_Fish* Fish) override;
	virtual void Shutdown() override;

private:
	ATC_Fish* CreatePooledFish(UWorld& World, TSubclassOf<ATC_Fish> FishClass, const FTransform& SpawnTransform);
	void DeactivatePooledFish(ATC_Fish* Fish);
	void ActivatePooledFish(ATC_Fish* Fish, const FTransform& SpawnTransform);
	int32 GetTotalPooledFishCount() const;

	TArray<TObjectPtr<ATC_Fish>> RegisteredFish;
	TMap<TSubclassOf<ATC_Fish>, TArray<TObjectPtr<ATC_Fish>>> InactivePoolByClass;
	TArray<TObjectPtr<ATC_Fish>> ActivePooledFish;
	bool bPoolInitialized = false;
};
