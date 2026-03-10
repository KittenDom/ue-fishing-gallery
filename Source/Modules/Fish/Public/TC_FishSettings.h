#pragma once

#include "Engine/DeveloperSettings.h"

#include "TC_FishSettings.generated.h"

USTRUCT(BlueprintType)
struct FTC_FishSpawnConfig
{
	GENERATED_BODY()

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSoftClassPtr<class ATC_Fish> FishClass;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Spawning", meta=(ClampMin="0"))
	int32 InitialPoolSize = 0;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Spawning", meta=(ClampMin="0"))
	int32 SpawnCount = 0;
};

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Fish Settings"))
class FISH_API UTC_FishSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetCategoryName() const override;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Pooling", meta=(ClampMin="0"))
	int32 MaxPoolSize = 100;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Spawning", meta=(ClampMin="0"))
	int32 DefaultSpawnCount = 10;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSoftClassPtr<class ATC_Fish> DefaultFishClass;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TArray<FTC_FishSpawnConfig> FishSpawnConfigs;
};
