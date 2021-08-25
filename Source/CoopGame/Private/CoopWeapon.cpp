// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "CoopGame/CoopGame.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"

// Sets default values
ACoopWeapon::ACoopWeapon()
{

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleFlashSocket";

	FireRate = 600;
}

void ACoopWeapon::BeginPlay()
{
	Super::BeginPlay();

	Time_FireRate = 60 / FireRate;
}

void ACoopWeapon::Fire() {

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

		FVector BeamEnd = TraceEnd;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams)) {
			AActor* HitActor = Hit.GetActor();
			
			BeamEnd = Hit.ImpactPoint;
			
			float ActualDamage = 20.0f;
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			if (SurfaceType == SURFACE_FLESHVULNERABLE) {
				ActualDamage = 100.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, EyeRotation.Vector(), Hit, MyOwner->GetInstigatorController(), this, DamageType);

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
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
		}
		//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);

		if (MuzzleEffect) {
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		}

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		if (BeamEffect) {
			UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamEffect, MuzzleLocation);
			if (TracerComp) {
				TracerComp->SetVectorParameter("BeamEnd", BeamEnd);
			}
		}

		APawn* MyPawn = Cast<APawn>(MyOwner);
		if (MyPawn) {
			APlayerController* PC = Cast<APlayerController>(MyPawn->GetController());
			if (PC) {
				PC->ClientStartCameraShake(FireCamShake);
			}
		}
	}
	
	LastTimeFired = GetWorld()->TimeSeconds;
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


