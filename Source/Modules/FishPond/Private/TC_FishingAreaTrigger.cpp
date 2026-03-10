// Copyright @subajat1 2026.


#include "TC_FishingAreaTrigger.h"

#include "TC_FishingAreaVolume.h"
#include "Components/BoxComponent.h"


// Sets default values
ATC_FishingAreaTrigger::ATC_FishingAreaTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	AreaBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaBounds"));
	RootComponent = AreaBounds;

	AreaBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaBounds->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaBounds->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AreaBounds->SetGenerateOverlapEvents(true);
	AreaBounds->OnComponentBeginOverlap.AddDynamic(this, &ATC_FishingAreaTrigger::HandleAreaBeginOverlap);
}

void ATC_FishingAreaTrigger::HandleAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!fishingArea)
	{
		UE_LOG(LogTemp, Error, TEXT("FishingArea is null!!!"));
		return;
	}

	if (fishingArea->IsInitialized())
	{
		UE_LOG(LogTemp, Log, TEXT("FishingArea has been initialized."));
		return;
	}

	fishingArea->InitializeFishingSpot();
}
