// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopHealthComponent.h"

// Sets default values for this component's properties
UCoopHealthComponent::UCoopHealthComponent()
{
	DefaultHealth = 100;
}


// Called when the game starts
void UCoopHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner) {
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UCoopHealthComponent::HandleTakeAnyDamage);
	}
	
	Health = DefaultHealth;
}

void UCoopHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0) {
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

