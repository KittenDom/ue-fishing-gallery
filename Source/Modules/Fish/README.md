# Fish

Core fish gameplay module.

Contains fish actors, shared fish data types, spawn and pooling contracts, and global fish spawning settings.

## Pooling Workflow

`UTC_FishPoolingSystem` is the public entry point for the module. It acts as a facade and delegates work to three internal services:

- `FTC_FishPoolService`: owns pooled fish lifecycle, active/inactive pools, and fish registration.
- `FTC_FishSpawnPlanner`: decides which fish classes should be spawned and builds spawn transforms.
- `FTC_FishAreaActivationService`: applies area requests by releasing old fish and activating the requested fish set for the area.

Sequence-style flow:

1. `FishPond` or another caller gets `UTC_FishPoolingSystem` from the world subsystem.
2. The caller invokes `EnsurePoolInitialized()` or `SetupActiveFishForArea(...)`.
3. `UTC_FishPoolingSystem` ensures its internal services exist.
4. `FTC_FishPoolService` initializes the inactive pool from `UTC_FishSettings` if it has not been initialized yet.
5. `FTC_FishAreaActivationService` asks `FTC_FishSpawnPlanner` for spawn transforms inside the requested area bounds.
6. For each spawn request, `UTC_FishPoolingSystem` / `FTC_FishPoolService` tries to acquire an existing inactive fish first.
7. If an inactive fish is available, it is reactivated, moved to the requested transform, and added to the active pool.
8. If no inactive fish is available and pool size is still under `MaxPoolSize`, a new fish actor is spawned and added to the active pool.
9. When a fish is no longer needed, `ReleaseFishToPool(...)` hides it, disables collision/tick, and moves it back to the inactive pool.

Pseudo sequence:

```text
FishingAreaVolume
  -> UTC_FishPoolingSystem.SetupActiveFishForArea()
  -> FTC_FishAreaActivationService.ActivateFishForArea()
  -> FTC_FishSpawnPlanner.BuildSpawnTransforms()
  -> FTC_FishPoolService.AcquireFish()
     -> reuse inactive fish OR spawn new fish
  -> active fish returned to caller
```

## Dummy Example

Simple example with one dummy fish class:

1. Create `BP_DummyFish` derived from `ATC_Fish`.
2. Assign a `FishDataRow` on that fish blueprint. The row can later come from a DataTable such as `DT_Fish`.
3. In `UTC_FishSettings`, configure:
- `DefaultFishClass = BP_DummyFish`
- `MaxPoolSize = 20`
- `FishSpawnConfigs = [{ FishClass = BP_DummyFish, InitialPoolSize = 5, SpawnCount = 3 }]`
4. A fishing area sends spawn requests such as:
- `BP_DummyFish x3`
5. The pooling system activates 3 fish inside the area:
- first from the inactive pool if available
- otherwise by spawning new `ATC_Fish` instances
6. Each `ATC_Fish` loads its metadata from `FishDataRow` during `BeginPlay()`.

That means the pool manages actor instances, while fish identity and tuning remain data-driven through the DataTable row assigned on each fish class or blueprint.
