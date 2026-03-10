#pragma once

#include "Components/SceneComponent.h"

#include "TC_FishingPole.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FISHERMEN_API UTC_FishingPole : public USceneComponent
{
	GENERATED_BODY()

public:
	UTC_FishingPole();
};
