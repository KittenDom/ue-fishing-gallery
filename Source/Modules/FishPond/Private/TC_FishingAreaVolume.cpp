#include "TC_FishingAreaVolume.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "TC_Fish.h"
#include "TC_FishPoolingSystem.h"
#include "TC_FishSpawnTypes.h"

ATC_FishingAreaVolume::ATC_FishingAreaVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	AreaBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaBounds"));
	RootComponent = AreaBounds;

	AreaBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaBounds->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaBounds->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AreaBounds->SetGenerateOverlapEvents(true);
	// AreaBounds->OnComponentBeginOverlap.AddDynamic(this, &ATC_FishingAreaVolume::HandleAreaBeginOverlap);
}

void ATC_FishingAreaVolume::InitializeFishingSpot()
{
	if (bIsInitialized)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	UTC_FishPoolingSystem* FishPoolingSystem = World->GetSubsystem<UTC_FishPoolingSystem>();
	if (!IsValid(FishPoolingSystem))
	{
		return;
	}

	TArray<FTC_FishSpawnRequest> SpawnRequests;
	for (const FTC_FishingAreaSpawnConfig& AreaSpawnConfig : AreaSpawnConfigs)
	{
		UClass* FishPawnClass = AreaSpawnConfig.FishPawnClass.LoadSynchronous();
		if (FishPawnClass == nullptr || !FishPawnClass->IsChildOf(ATC_Fish::StaticClass()))
		{
			continue;
		}

		FTC_FishSpawnRequest& SpawnRequest = SpawnRequests.AddDefaulted_GetRef();
		SpawnRequest.FishClass = TSoftClassPtr<ATC_Fish>(AreaSpawnConfig.FishPawnClass.ToSoftObjectPath());
		SpawnRequest.SpawnCount = AreaSpawnConfig.SpawnCount;
	}

	FishPoolingSystem->EnsurePoolInitialized();
	FishPoolingSystem->SetupActiveFishForArea(SpawnRequests, AreaBounds->Bounds.GetBox());

	bIsInitialized = true;
}

bool ATC_FishingAreaVolume::ContainsLocation(const FVector& WorldLocation) const
{
	return AreaBounds->Bounds.GetBox().IsInsideOrOn(WorldLocation);
}

bool ATC_FishingAreaVolume::IsInitialized() const
{
	return bIsInitialized;
}

void ATC_FishingAreaVolume::HandleAreaBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	const APawn* OverlappingPawn = Cast<APawn>(OtherActor);
	if (!IsValid(OverlappingPawn) || !OverlappingPawn->IsPlayerControlled())
	{
		return;
	}
}
