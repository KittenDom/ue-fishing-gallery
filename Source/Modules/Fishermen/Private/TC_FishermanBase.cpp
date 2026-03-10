#include "TC_FishermanBase.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TC_FishingFSMComponent.h"
#include "TC_FishingPole.h"

ATC_FishermanBase::ATC_FishermanBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->SocketOffset = FVector(0.0f, 0.0f, 50.0f);
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 15.0f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	CameraComponent->bUsePawnControlRotation = true;

	FishingPoleComponent = CreateDefaultSubobject<UTC_FishingPole>(TEXT("FishingPole"));
	FishingPoleComponent->SetupAttachment(RootComponent);

	FishingFSMComponent = CreateDefaultSubobject<UTC_FishingFSMComponent>(TEXT("FishingFSM"));
}

void ATC_FishermanBase::BeginPlay()
{
	Super::BeginPlay();
}

void ATC_FishermanBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATC_FishermanBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
