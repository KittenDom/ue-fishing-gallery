#include "TC_Fish.h"

#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "TC_FishPoolingSystem.h"

ATC_Fish::ATC_Fish()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootSceneComponent;

	FishType = NAME_None;
	FishSize = 0.0f;
	MorningAppearChance = 0.0f;
	AfternoonAppearChance = 0.0f;
	EveningAppearChance = 0.0f;
	WeatherType = ETC_WeatherType::Clear;
	SeasonType = ETC_SeasonType::Spring;
	FishingAreaType = ETC_FishingAreaType::Pond;
}

void ATC_Fish::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyDataFromRow();
}

void ATC_Fish::BeginPlay()
{
	Super::BeginPlay();
	ApplyDataFromRow();

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	UTC_FishPoolingSystem* FishPoolingSystem = World->GetSubsystem<UTC_FishPoolingSystem>();
	if (!IsValid(FishPoolingSystem))
	{
		return;
	}

	FishPoolingSystem->RegisterFish(this);
}

void ATC_Fish::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		Super::EndPlay(EndPlayReason);
		return;
	}

	UTC_FishPoolingSystem* FishPoolingSystem = World->GetSubsystem<UTC_FishPoolingSystem>();
	if (IsValid(FishPoolingSystem))
	{
		FishPoolingSystem->UnregisterFish(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ATC_Fish::ApplyDataFromRow()
{
	static const FString ContextString(TEXT("TC_Fish::ApplyDataFromRow"));
	const FTC_FishDataRow* DataRow = FishDataRow.GetRow<FTC_FishDataRow>(ContextString);
	if (DataRow == nullptr)
	{
		return;
	}

	FishType = DataRow->FishType;
	FishSize = DataRow->FishSize;
	MorningAppearChance = DataRow->MorningAppearChance;
	AfternoonAppearChance = DataRow->AfternoonAppearChance;
	EveningAppearChance = DataRow->EveningAppearChance;
	WeatherType = DataRow->WeatherType;
	SeasonType = DataRow->SeasonType;
	FishingAreaType = DataRow->FishingAreaType;
}
