#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "TC_FishSpawnTypes.h"

#include "TC_FishPoolingSystem.generated.h"

UCLASS()
class FISH_API UTC_FishPoolingSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual ~UTC_FishPoolingSystem() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;

	void RegisterFish(class ATC_Fish* Fish);
	void UnregisterFish(class ATC_Fish* Fish);

	UFUNCTION(BlueprintCallable, Category = "Fishing|Subsystem")
	TArray<class ATC_Fish*> GetFishInBox(const FBox& Bounds) const;

	UFUNCTION(BlueprintCallable, Category = "Fishing|Subsystem")
	class ATC_Fish* SpawnFishFromPool(const FTransform& SpawnTransform, TSubclassOf<class ATC_Fish> FishClass);

	UFUNCTION(BlueprintCallable, Category = "Fishing|Subsystem")
	void ReleaseFishToPool(class ATC_Fish* Fish);

	UFUNCTION(BlueprintCallable, Category = "Fishing|Subsystem")
	TArray<class ATC_Fish*> SpawnFishFromSettings(const TArray<FTransform>& SpawnTransforms);

	UFUNCTION(BlueprintCallable, Category = "Fishing|Subsystem")
	void EnsurePoolInitialized();

	UFUNCTION(BlueprintPure, Category = "Fishing|Subsystem")
	bool IsPoolInitialized() const;

	UFUNCTION(BlueprintCallable, Category = "Fishing|Subsystem")
	TArray<class ATC_Fish*> SetupActiveFishForArea(const TArray<FTC_FishSpawnRequest>& SpawnRequests, const FBox& AreaBounds);

private:
	void EnsureServices();
	const class UTC_FishSettings* GetSettings() const;

	class ITC_FishPoolService* PoolService = nullptr;
	class ITC_FishSpawnPlanner* SpawnPlanner = nullptr;
	class ITC_FishAreaActivationService* AreaActivationService = nullptr;
};
