#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "TC_FishDataTypes.generated.h"

UENUM(BlueprintType)
enum class ETC_TimeOfDay : uint8
{
	Morning UMETA(DisplayName = "Morning"),
	Afternoon UMETA(DisplayName = "Afternoon"),
	Evening UMETA(DisplayName = "Evening")
};

UENUM(BlueprintType)
enum class ETC_WeatherType : uint8
{
	Clear UMETA(DisplayName = "Clear"),
	Rainy UMETA(DisplayName = "Rainy"),
	Stormy UMETA(DisplayName = "Stormy")
};

UENUM(BlueprintType)
enum class ETC_SeasonType : uint8
{
	Spring UMETA(DisplayName = "Spring"),
	Summer UMETA(DisplayName = "Summer"),
	Autumn UMETA(DisplayName = "Autumn"),
	Winter UMETA(DisplayName = "Winter")
};

UENUM(BlueprintType)
enum class ETC_FishingAreaType : uint8
{
	River UMETA(DisplayName = "River"),
	Lake UMETA(DisplayName = "Lake"),
	Pond UMETA(DisplayName = "Pond"),
	Sea UMETA(DisplayName = "Sea")
};

USTRUCT(BlueprintType)
struct FTC_FishDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fish")
	FName FishType = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fish")
	float FishSize = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fish|Appearance")
	float MorningAppearChance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fish|Appearance")
	float AfternoonAppearChance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fish|Appearance")
	float EveningAppearChance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fish|Environment")
	ETC_WeatherType WeatherType = ETC_WeatherType::Clear;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fish|Environment")
	ETC_SeasonType SeasonType = ETC_SeasonType::Spring;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fish|Environment")
	ETC_FishingAreaType FishingAreaType = ETC_FishingAreaType::Pond;
};
