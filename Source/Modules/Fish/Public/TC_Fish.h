#pragma once

#include "GameFramework/Pawn.h"
#include "TC_FishDataTypes.h"

#include "TC_Fish.generated.h"

UCLASS()
class FISH_API ATC_Fish : public APawn
{
	GENERATED_BODY()

public:
	ATC_Fish();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void ApplyDataFromRow();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fish|Data", meta=(AllowPrivateAccess=true))
	FDataTableRowHandle FishDataRow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fish", meta=(AllowPrivateAccess=true))
	class USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fish", meta=(AllowPrivateAccess=true))
	FName FishType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fish", meta=(AllowPrivateAccess=true))
	float FishSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fish|Appearance", meta=(AllowPrivateAccess=true))
	float MorningAppearChance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fish|Appearance", meta=(AllowPrivateAccess=true))
	float AfternoonAppearChance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fish|Appearance", meta=(AllowPrivateAccess=true))
	float EveningAppearChance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fish|Environment", meta=(AllowPrivateAccess=true))
	ETC_WeatherType WeatherType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fish|Environment", meta=(AllowPrivateAccess=true))
	ETC_SeasonType SeasonType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fish|Environment", meta=(AllowPrivateAccess=true))
	ETC_FishingAreaType FishingAreaType;
};
