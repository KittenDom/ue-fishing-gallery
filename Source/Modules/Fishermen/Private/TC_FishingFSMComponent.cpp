#include "TC_FishingFSMComponent.h"

UTC_FishingFSMComponent::UTC_FishingFSMComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentState = ETC_FishingState::Idle;
}

void UTC_FishingFSMComponent::SetState(ETC_FishingState NewState)
{
	CurrentState = NewState;
}

ETC_FishingState UTC_FishingFSMComponent::GetState() const
{
	return CurrentState;
}

bool UTC_FishingFSMComponent::IsInState(ETC_FishingState InState) const
{
	return CurrentState == InState;
}
