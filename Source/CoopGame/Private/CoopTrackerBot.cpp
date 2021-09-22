// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "CoopHealthComponent.h"

// Sets default values
ACoopTrackerBot::ACoopTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);

	HealthComp = CreateDefaultSubobject<UCoopHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ACoopTrackerBot::HandleDamage);

	bUseVelocityChange = false;
	MovementForce = 1000;
	RequiredDistanceTarget = 100;
}

// Called when the game starts or when spawned
void ACoopTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	NextPathPoint = GetNextPathPoint();
}

// Called every frame
void ACoopTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceTarget = (GetActorLocation() - NextPathPoint).Size();
	if (DistanceTarget <= RequiredDistanceTarget) {
		NextPathPoint = GetNextPathPoint();
	}
	else {
		FVector ForceDir = NextPathPoint - GetActorLocation();
		ForceDir.GetSafeNormal();
		ForceDir.Normalize();
		ForceDir *= MovementForce;

		MeshComp->AddForce(ForceDir, NAME_None, bUseVelocityChange);
	}
}

FVector ACoopTrackerBot::GetNextPathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath->PathPoints.Num() > 1) {
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();
}

void ACoopTrackerBot::HandleDamage(UCoopHealthComponent* NotHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (MatInst == nullptr) {
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst) {
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}
}
