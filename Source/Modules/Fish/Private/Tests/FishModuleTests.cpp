#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/Package.h"

#define private public
#define protected public
#include "TC_Fish.h"
#include "TC_FishPoolingSystem.h"
#undef protected
#undef private

#include "TC_FishAreaActivationService.h"
#include "TC_FishPoolService.h"
#include "TC_FishSettings.h"
#include "TC_FishSpawnPlanner.h"

namespace
{
	struct FTestWorldScope
	{
		UWorld* World = nullptr;

		bool Create(FAutomationTestBase& Test)
		{
			if (GEngine == nullptr)
			{
				Test.AddError(TEXT("GEngine is not available."));
				return false;
			}

			static int32 WorldCounter = 0;
			const FName WorldName(*FString::Printf(TEXT("FishTestWorld_%d"), ++WorldCounter));
			UPackage* WorldPackage = CreatePackage(*FString::Printf(TEXT("/Engine/Transient/%s"), *WorldName.ToString()));

			FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
			const UWorld::InitializationValues InitializationValues = UWorld::InitializationValues()
				.AllowAudioPlayback(false)
				.RequiresHitProxies(false)
				.CreatePhysicsScene(false)
				.CreateNavigation(false)
				.CreateAISystem(false)
				.ShouldSimulatePhysics(false)
				.EnableTraceCollision(false)
				.SetTransactional(false);
			World = UWorld::CreateWorld(EWorldType::Game, false, WorldName, WorldPackage, true, ERHIFeatureLevel::Num, &InitializationValues, false);
			if (!Test.TestNotNull(TEXT("World should be created"), World))
			{
				return false;
			}

			WorldContext.SetCurrentWorld(World);
			World->BeginPlay();
			return true;
		}

		~FTestWorldScope()
		{
			if (World == nullptr)
			{
				return;
			}

			if (GEngine != nullptr)
			{
				GEngine->DestroyWorldContext(World);
			}

			World->DestroyWorld(false);
			World->RemoveFromRoot();
			World = nullptr;
		}
	};

	class FMockFishPoolService final : public ITC_FishPoolService
	{
	public:
		int32 RegisterCalls = 0;
		int32 UnregisterCalls = 0;
		int32 GetFishInBoxCalls = 0;
		int32 InitializeCalls = 0;
		int32 AcquireCalls = 0;
		int32 ReleaseCalls = 0;
		int32 ShutdownCalls = 0;
		bool bInitialized = false;
		TArray<ATC_Fish*> FishInBoxResult;
		TArray<ATC_Fish*> AcquireResults;
		TArray<TSubclassOf<ATC_Fish>> RequestedClasses;

		virtual void RegisterFish(ATC_Fish* Fish) override
		{
			++RegisterCalls;
		}

		virtual void UnregisterFish(ATC_Fish* Fish) override
		{
			++UnregisterCalls;
		}

		virtual TArray<ATC_Fish*> GetFishInBox(const FBox& Bounds) const override
		{
			const_cast<FMockFishPoolService*>(this)->GetFishInBoxCalls++;
			return FishInBoxResult;
		}

		virtual bool IsInitialized() const override
		{
			return bInitialized;
		}

		virtual void Initialize(UWorld& World, const UTC_FishSettings& Settings) override
		{
			++InitializeCalls;
			bInitialized = true;
		}

		virtual ATC_Fish* AcquireFish(UWorld& World, const UTC_FishSettings& Settings, TSubclassOf<ATC_Fish> FishClass, const FTransform& SpawnTransform) override
		{
			++AcquireCalls;
			RequestedClasses.Add(FishClass);
			if (AcquireResults.IsEmpty())
			{
				return nullptr;
			}

			return AcquireResults[AcquireCalls - 1];
		}

		virtual void ReleaseFish(ATC_Fish* Fish) override
		{
			++ReleaseCalls;
		}

		virtual void Shutdown() override
		{
			++ShutdownCalls;
		}
	};

	class FMockFishSpawnPlanner final : public ITC_FishSpawnPlanner
	{
	public:
		int32 ResolveCalls = 0;
		int32 BuildRequestsCalls = 0;
		int32 BuildTransformsCalls = 0;
		TSubclassOf<ATC_Fish> ResolvedClass;
		TArray<FTC_FishSpawnRequest> SpawnRequestsFromSettings;
		TArray<FTransform> TransformResults;

		virtual TSubclassOf<ATC_Fish> ResolveFishClass(const UTC_FishSettings& Settings, TSubclassOf<ATC_Fish> RequestedClass) const override
		{
			const_cast<FMockFishSpawnPlanner*>(this)->ResolveCalls++;
			return ResolvedClass ? ResolvedClass : RequestedClass;
		}

		virtual TArray<FTC_FishSpawnRequest> BuildSpawnRequestsFromSettings(const UTC_FishSettings& Settings) const override
		{
			const_cast<FMockFishSpawnPlanner*>(this)->BuildRequestsCalls++;
			return SpawnRequestsFromSettings;
		}

		virtual TArray<FTransform> BuildSpawnTransforms(const FBox& AreaBounds, int32 SpawnCount) const override
		{
			const_cast<FMockFishSpawnPlanner*>(this)->BuildTransformsCalls++;
			return TransformResults;
		}
	};

	class FMockFishAreaActivationService final : public ITC_FishAreaActivationService
	{
	public:
		int32 ActivateCalls = 0;
		TArray<ATC_Fish*> ActivatedFishToReturn;
		TArray<FTC_FishSpawnRequest> ReceivedRequests;

		virtual TArray<ATC_Fish*> ActivateFishForArea(
			UWorld& World,
			const UTC_FishSettings& Settings,
			ITC_FishPoolService& PoolService,
			const ITC_FishSpawnPlanner& SpawnPlanner,
			const TArray<FTC_FishSpawnRequest>& SpawnRequests,
			const FBox& AreaBounds) const override
		{
			const_cast<FMockFishAreaActivationService*>(this)->ActivateCalls++;
			const_cast<FMockFishAreaActivationService*>(this)->ReceivedRequests = SpawnRequests;
			return ActivatedFishToReturn;
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFishSpawnPlannerResolveRequestedClassTest, "Fish.Unit.SpawnPlanner.ResolveRequestedClass", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFishSpawnPlannerResolveRequestedClassTest::RunTest(const FString& Parameters)
{
	FTC_FishSpawnPlanner Planner;
	UTC_FishSettings* Settings = NewObject<UTC_FishSettings>();
	Settings->DefaultFishClass = ATC_Fish::StaticClass();

	const TSubclassOf<ATC_Fish> Result = Planner.ResolveFishClass(*Settings, ATC_Fish::StaticClass());
	TestEqual(TEXT("Requested class should be returned as-is"), Result, TSubclassOf<ATC_Fish>(ATC_Fish::StaticClass()));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFishSpawnPlannerResolveDefaultClassTest, "Fish.Unit.SpawnPlanner.ResolveDefaultClass", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFishSpawnPlannerResolveDefaultClassTest::RunTest(const FString& Parameters)
{
	FTC_FishSpawnPlanner Planner;
	UTC_FishSettings* Settings = NewObject<UTC_FishSettings>();
	Settings->DefaultFishClass = ATC_Fish::StaticClass();

	const TSubclassOf<ATC_Fish> Result = Planner.ResolveFishClass(*Settings, nullptr);
	TestEqual(TEXT("Default class should be returned when request is null"), Result, TSubclassOf<ATC_Fish>(ATC_Fish::StaticClass()));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFishSpawnPlannerBuildRequestsFromConfigTest, "Fish.Unit.SpawnPlanner.BuildRequestsFromConfig", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFishSpawnPlannerBuildRequestsFromConfigTest::RunTest(const FString& Parameters)
{
	FTC_FishSpawnPlanner Planner;
	UTC_FishSettings* Settings = NewObject<UTC_FishSettings>();

	FTC_FishSpawnConfig& Config = Settings->FishSpawnConfigs.AddDefaulted_GetRef();
	Config.FishClass = ATC_Fish::StaticClass();
	Config.SpawnCount = 3;

	const TArray<FTC_FishSpawnRequest> SpawnRequests = Planner.BuildSpawnRequestsFromSettings(*Settings);
	TestEqual(TEXT("One valid request should be produced"), SpawnRequests.Num(), 1);
	TestEqual(TEXT("Spawn count should match config"), SpawnRequests[0].SpawnCount, 3);
	TestTrue(TEXT("Fish class should match config"), SpawnRequests[0].FishClass.LoadSynchronous() == ATC_Fish::StaticClass());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFishSpawnPlannerBuildDefaultRequestTest, "Fish.Unit.SpawnPlanner.BuildDefaultRequest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFishSpawnPlannerBuildDefaultRequestTest::RunTest(const FString& Parameters)
{
	FTC_FishSpawnPlanner Planner;
	UTC_FishSettings* Settings = NewObject<UTC_FishSettings>();
	Settings->DefaultFishClass = ATC_Fish::StaticClass();
	Settings->DefaultSpawnCount = 2;

	const TArray<FTC_FishSpawnRequest> SpawnRequests = Planner.BuildSpawnRequestsFromSettings(*Settings);
	TestEqual(TEXT("Default request should be produced when no explicit configs exist"), SpawnRequests.Num(), 1);
	TestEqual(TEXT("Default spawn count should be used"), SpawnRequests[0].SpawnCount, 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFishSpawnPlannerBuildTransformsTest, "Fish.Unit.SpawnPlanner.BuildTransforms", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFishSpawnPlannerBuildTransformsTest::RunTest(const FString& Parameters)
{
	FTC_FishSpawnPlanner Planner;
	const FBox AreaBounds(FVector(-10.0f, -20.0f, -5.0f), FVector(10.0f, 20.0f, 5.0f));

	const TArray<FTransform> SpawnTransforms = Planner.BuildSpawnTransforms(AreaBounds, 5);
	TestEqual(TEXT("Requested number of spawn transforms should be produced"), SpawnTransforms.Num(), 5);

	for (const FTransform& SpawnTransform : SpawnTransforms)
	{
		TestTrue(TEXT("Spawn transform should be inside bounds"), AreaBounds.IsInsideOrOn(SpawnTransform.GetLocation()));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFishAreaActivationServiceTest, "Fish.Unit.AreaActivationService.ActivatesAndReleases", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFishAreaActivationServiceTest::RunTest(const FString& Parameters)
{
	FTestWorldScope WorldScope;
	if (!WorldScope.Create(*this))
	{
		return false;
	}

	FTC_FishAreaActivationService ActivationService;
	FMockFishPoolService PoolService;
	FMockFishSpawnPlanner SpawnPlanner;
	UTC_FishSettings* Settings = NewObject<UTC_FishSettings>();

	ATC_Fish* ExistingFish = WorldScope.World->SpawnActor<ATC_Fish>();
	ATC_Fish* SpawnedFishA = WorldScope.World->SpawnActor<ATC_Fish>();
	ATC_Fish* SpawnedFishB = WorldScope.World->SpawnActor<ATC_Fish>();
	PoolService.FishInBoxResult = { ExistingFish };
	PoolService.AcquireResults = { SpawnedFishA, SpawnedFishB };
	SpawnPlanner.TransformResults = { FTransform::Identity, FTransform(FVector(100.0f, 0.0f, 0.0f)) };

	FTC_FishSpawnRequest Request;
	Request.FishClass = ATC_Fish::StaticClass();
	Request.SpawnCount = 2;

	const TArray<ATC_Fish*> ActivatedFish = ActivationService.ActivateFishForArea(
		*WorldScope.World,
		*Settings,
		PoolService,
		SpawnPlanner,
		{ Request },
		FBox(FVector::ZeroVector, FVector(200.0f)));

	TestEqual(TEXT("Pool should initialize once when needed"), PoolService.InitializeCalls, 1);
	TestEqual(TEXT("Existing fish should be released"), PoolService.ReleaseCalls, 1);
	TestEqual(TEXT("Two fish should be acquired"), PoolService.AcquireCalls, 2);
	TestEqual(TEXT("Two activated fish should be returned"), ActivatedFish.Num(), 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFishPoolServiceAcquireReleaseReuseTest, "Fish.Unit.PoolService.AcquireReleaseReuse", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFishPoolServiceAcquireReleaseReuseTest::RunTest(const FString& Parameters)
{
	FTestWorldScope WorldScope;
	if (!WorldScope.Create(*this))
	{
		return false;
	}

	FTC_FishPoolService PoolService;
	UTC_FishSettings* Settings = NewObject<UTC_FishSettings>();
	Settings->MaxPoolSize = 2;

	FTC_FishSpawnConfig& Config = Settings->FishSpawnConfigs.AddDefaulted_GetRef();
	Config.FishClass = ATC_Fish::StaticClass();
	Config.InitialPoolSize = 1;
	Config.SpawnCount = 1;

	PoolService.Initialize(*WorldScope.World, *Settings);
	TestTrue(TEXT("Pool should report initialized"), PoolService.IsInitialized());

	ATC_Fish* FirstFish = PoolService.AcquireFish(*WorldScope.World, *Settings, ATC_Fish::StaticClass(), FTransform(FVector(10.0f, 0.0f, 0.0f)));
	TestNotNull(TEXT("First fish should be acquired from pool"), FirstFish);
	TestEqual(TEXT("Fish in box should include active fish"), PoolService.GetFishInBox(FBox(FVector::ZeroVector, FVector(20.0f))).Num(), 1);

	PoolService.ReleaseFish(FirstFish);
	ATC_Fish* ReusedFish = PoolService.AcquireFish(*WorldScope.World, *Settings, ATC_Fish::StaticClass(), FTransform(FVector(15.0f, 0.0f, 0.0f)));
	TestEqual(TEXT("Released fish should be reused"), ReusedFish, FirstFish);

	PoolService.Shutdown();
	TestFalse(TEXT("Pool should report not initialized after shutdown"), PoolService.IsInitialized());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFishPoolServiceMaxPoolSizeTest, "Fish.Unit.PoolService.MaxPoolSize", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFishPoolServiceMaxPoolSizeTest::RunTest(const FString& Parameters)
{
	FTestWorldScope WorldScope;
	if (!WorldScope.Create(*this))
	{
		return false;
	}

	FTC_FishPoolService PoolService;
	UTC_FishSettings* Settings = NewObject<UTC_FishSettings>();
	Settings->MaxPoolSize = 1;

	ATC_Fish* FirstFish = PoolService.AcquireFish(*WorldScope.World, *Settings, ATC_Fish::StaticClass(), FTransform::Identity);
	ATC_Fish* SecondFish = PoolService.AcquireFish(*WorldScope.World, *Settings, ATC_Fish::StaticClass(), FTransform(FVector(50.0f, 0.0f, 0.0f)));

	TestNotNull(TEXT("First fish should be created"), FirstFish);
	TestNull(TEXT("Second fish should be blocked by max pool size"), SecondFish);
	PoolService.Shutdown();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFishPoolingSystemDelegationTest, "Fish.Unit.FishPoolingSystem.DelegatesToServices", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFishPoolingSystemDelegationTest::RunTest(const FString& Parameters)
{
	FTestWorldScope WorldScope;
	if (!WorldScope.Create(*this))
	{
		return false;
	}

	UTC_FishPoolingSystem* Subsystem = NewObject<UTC_FishPoolingSystem>(WorldScope.World);
	FMockFishPoolService* PoolService = new FMockFishPoolService();
	FMockFishSpawnPlanner* SpawnPlanner = new FMockFishSpawnPlanner();
	FMockFishAreaActivationService* AreaActivationService = new FMockFishAreaActivationService();

	ATC_Fish* ReturnedFish = WorldScope.World->SpawnActor<ATC_Fish>();
	PoolService->AcquireResults = { ReturnedFish };
	SpawnPlanner->ResolvedClass = ATC_Fish::StaticClass();
	AreaActivationService->ActivatedFishToReturn = { ReturnedFish };

	Subsystem->PoolService = PoolService;
	Subsystem->SpawnPlanner = SpawnPlanner;
	Subsystem->AreaActivationService = AreaActivationService;

	ATC_Fish* SpawnedFish = Subsystem->SpawnFishFromPool(FTransform::Identity, ATC_Fish::StaticClass());
	TestEqual(TEXT("Spawn should come from pool service"), SpawnedFish, ReturnedFish);
	TestEqual(TEXT("Spawn planner should resolve class once"), SpawnPlanner->ResolveCalls, 1);
	TestEqual(TEXT("Pool service should acquire once"), PoolService->AcquireCalls, 1);

	FTC_FishSpawnRequest Request;
	Request.FishClass = ATC_Fish::StaticClass();
	Request.SpawnCount = 1;
	const TArray<ATC_Fish*> ActivatedFish = Subsystem->SetupActiveFishForArea({ Request }, FBox(FVector::ZeroVector, FVector(100.0f)));
	TestEqual(TEXT("Area activation should delegate once"), AreaActivationService->ActivateCalls, 1);
	TestEqual(TEXT("Area activation result should be returned"), ActivatedFish.Num(), 1);

	Subsystem->Deinitialize();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFishApplyDataFromRowTest, "Fish.Unit.Fish.ApplyDataFromRow", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFishApplyDataFromRowTest::RunTest(const FString& Parameters)
{
	FTestWorldScope WorldScope;
	if (!WorldScope.Create(*this))
	{
		return false;
	}

	UDataTable* DataTable = NewObject<UDataTable>(GetTransientPackage());
	DataTable->RowStruct = FTC_FishDataRow::StaticStruct();

	FTC_FishDataRow Row;
	Row.FishType = TEXT("DummyFish");
	Row.FishSize = 42.0f;
	Row.MorningAppearChance = 0.1f;
	Row.AfternoonAppearChance = 0.2f;
	Row.EveningAppearChance = 0.3f;
	Row.WeatherType = ETC_WeatherType::Rainy;
	Row.SeasonType = ETC_SeasonType::Autumn;
	Row.FishingAreaType = ETC_FishingAreaType::Lake;
	DataTable->AddRow(TEXT("DummyRow"), Row);

	ATC_Fish* Fish = WorldScope.World->SpawnActorDeferred<ATC_Fish>(ATC_Fish::StaticClass(), FTransform::Identity);
	if (!TestNotNull(TEXT("Deferred fish should be created"), Fish))
	{
		return false;
	}

	Fish->FishDataRow.DataTable = DataTable;
	Fish->FishDataRow.RowName = TEXT("DummyRow");
	Fish->FinishSpawning(FTransform::Identity);

	TestEqual(TEXT("Fish type should be copied from row"), Fish->FishType, FName(TEXT("DummyFish")));
	TestEqual(TEXT("Fish size should be copied from row"), Fish->FishSize, 42.0f);
	TestEqual(TEXT("Morning chance should be copied from row"), Fish->MorningAppearChance, 0.1f);
	TestEqual(TEXT("Afternoon chance should be copied from row"), Fish->AfternoonAppearChance, 0.2f);
	TestEqual(TEXT("Evening chance should be copied from row"), Fish->EveningAppearChance, 0.3f);
	TestEqual(TEXT("Weather type should be copied from row"), Fish->WeatherType, ETC_WeatherType::Rainy);
	TestEqual(TEXT("Season type should be copied from row"), Fish->SeasonType, ETC_SeasonType::Autumn);
	TestEqual(TEXT("Fishing area type should be copied from row"), Fish->FishingAreaType, ETC_FishingAreaType::Lake);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFishApplyDataFromMissingRowTest, "Fish.Unit.Fish.ApplyDataFromMissingRow", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFishApplyDataFromMissingRowTest::RunTest(const FString& Parameters)
{
	FTestWorldScope WorldScope;
	if (!WorldScope.Create(*this))
	{
		return false;
	}

	ATC_Fish* Fish = WorldScope.World->SpawnActor<ATC_Fish>();
	if (!TestNotNull(TEXT("Fish should be spawned"), Fish))
	{
		return false;
	}

	TestEqual(TEXT("Fish type should remain default when row is missing"), Fish->FishType, NAME_None);
	TestEqual(TEXT("Fish size should remain default when row is missing"), Fish->FishSize, 0.0f);
	TestEqual(TEXT("Weather type should remain default when row is missing"), Fish->WeatherType, ETC_WeatherType::Clear);
	return true;
}

#endif
