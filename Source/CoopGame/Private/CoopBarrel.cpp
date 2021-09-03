// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopBarrel.h"
#include "CoopHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ACoopBarrel::ACoopBarrel()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<UCoopHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ACoopBarrel::OnHealthChanged);

	ForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("ForceComp"));
	ForceComp->SetupAttachment(MeshComp);
	ForceComp->Radius = 250;
	ForceComp->bImpulseVelChange = true;
	ForceComp->bAutoActivate = false;
	ForceComp->bIgnoreOwningActor = true;
}


void ACoopBarrel::OnHealthChanged(UCoopHealthComponent* NotHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bExploded) {

		bExploded = true;

		FVector BarrelLocation = GetActorLocation();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
		
		ForceComp->FireImpulse();

		Destroy();
	}
}

