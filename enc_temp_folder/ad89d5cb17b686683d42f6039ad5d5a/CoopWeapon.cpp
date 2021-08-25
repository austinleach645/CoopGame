// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "CoopGame/CoopGame.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

// Sets default values
ACoopWeapon::ACoopWeapon()
{

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleFlashSocket";
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

		FVector BeamEnd = TraceEnd;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams)) {
			AActor* HitActor = Hit.GetActor();
			
			BeamEnd = Hit.ImpactPoint;

			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, EyeRotation.Vector(), Hit, MyOwner->GetInstigatorController() , this, DamageType);

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
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

}


