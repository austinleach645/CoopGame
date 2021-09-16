// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "CoopGame/CoopGame.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACoopWeapon::ACoopWeapon()
{

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleFlashSocket";

	FireRate = 600;

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

void ACoopWeapon::BeginPlay()
{
	Super::BeginPlay();

	Time_FireRate = 60 / FireRate;
}

void ACoopWeapon::Fire() {

	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}

	AActor* MyOwner = GetOwner();
	if (MyOwner) {
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * 1000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		BeamEnd = TraceEnd;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams)) {
			AActor* HitActor = Hit.GetActor();

			BeamEnd = Hit.ImpactPoint;

			float ActualDamage = 20.0f;
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			if (SurfaceType == SURFACE_FLESHVULNERABLE) {
				ActualDamage = 100.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, EyeRotation.Vector(), Hit, MyOwner->GetInstigatorController(), this, DamageType);

			PlayHitEffects(SurfaceType, BeamEnd);
		}
		//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);

		PlayWeaponEffects(BeamEnd);

		if (GetLocalRole() == ROLE_Authority)
		{
			HitScanTrace.a++;
			HitScanTrace.TraceEnd = BeamEnd;
			HitScanTrace.SurfaceType = SurfaceType;
		}

		LastTimeFired = GetWorld()->TimeSeconds;
	}
}

void ACoopWeapon::PlayWeaponEffects(FVector TraceEnd)
{
	if (MuzzleEffect) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

	if (BeamEffect) {
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamEffect, MuzzleLocation);
		if (TracerComp) {
			TracerComp->SetVectorParameter("BeamEnd", TraceEnd);
		}
	}
	AActor* MyOwner = GetOwner();
	APawn* MyPawn = Cast<APawn>(MyOwner);
	if (MyPawn) {
		APlayerController* PC = Cast<APlayerController>(MyPawn->GetController());
		if (PC) {
			PC->ClientStartCameraShake(FireCamShake);
		}
	}
}

void ACoopWeapon::PlayHitEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
		SelectedEffect = BodyEffect;
		break;
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = HeadEffect;
		break;
	default:
		SelectedEffect = ImpactEffect;
		break;
	}

	if (SelectedEffect) {
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void ACoopWeapon::ServerFire_Implementation()
{
	Fire();
}
bool ACoopWeapon::ServerFire_Validate()
{
	return true;
}

void ACoopWeapon::StartFire()
{
	float FirstDelay = LastTimeFired + Time_FireRate - GetWorld()->TimeSeconds;

	GetWorldTimerManager().SetTimer(TimeBetweenShots, this, &ACoopWeapon::Fire, Time_FireRate, true, FirstDelay);
}

void ACoopWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimeBetweenShots);
}

void ACoopWeapon::OnRep_HitScanTrace() 
{
	PlayWeaponEffects(HitScanTrace.TraceEnd);

	PlayHitEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceEnd);
}

void ACoopWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACoopWeapon, HitScanTrace, COND_SkipOwner);
}


