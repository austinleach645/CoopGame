// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CoopCharacter.generated.h"

class ACoopWeapon;
class UCameraComponent;
class USpringArmComponent;
class UCoopHealthComponent;

UCLASS()
class COOPGAME_API ACoopCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACoopCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);

	void MoveRight(float value);

	void BeginCrouch();

	void EndCrouch();

	void ZoomIn();

	void ZoomOut();

	void Fire();

	void StopFire();

	ACoopWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ACoopWeapon> StarterWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraComp")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraComp")
	USpringArmComponent* SpringComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HealthComp")
	UCoopHealthComponent* HealthComp;

	bool bWantsToZoom;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bDied;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float ZoomFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	float DefaultFOV;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	UFUNCTION()
	void OnHealthChanged(UCoopHealthComponent* NotHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

};
