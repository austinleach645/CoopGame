// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CoopTrackerBot.generated.h"

class UStaticMeshComponent;
class UCoopHealthComponent;
class USphereComponent;

UCLASS()
class COOPGAME_API ACoopTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACoopTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	FVector GetNextPathPoint();

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float RequiredDistanceTarget;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UCoopHealthComponent* HealthComp;

	UFUNCTION()
	void HandleDamage(UCoopHealthComponent* NotHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UMaterialInstanceDynamic* MatInst;

	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	UParticleSystem* ExplosionEffect;

	bool bExploded;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float ExplodeRadius;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float ExplodeDamage;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComp;

	FTimerHandle TimerHandle_SelfDamage;

	void DamageSelf();

	bool bStartSelfDestruct;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
