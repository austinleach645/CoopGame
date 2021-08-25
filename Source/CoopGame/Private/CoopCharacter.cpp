// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
ACoopCharacter::ACoopCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringComp"));
	SpringComp->bUsePawnControlRotation = true;
	SpringComp->SetupAttachment(RootComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringComp);

}

// Called when the game starts or when spawned
void ACoopCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = CameraComp->FieldOfView;
}

void ACoopCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector() * value);
}

void ACoopCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector() * value);
}

void ACoopCharacter::BeginCrouch() {
	Crouch();
}

void ACoopCharacter::EndCrouch() {
	UnCrouch();
}

void ACoopCharacter::ZoomIn() {
	bWantsToZoom = true;
}

void ACoopCharacter::ZoomOut() {
	bWantsToZoom = false;
}

// Called every frame
void ACoopCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomFOV : DefaultFOV;
	float CurrentFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(CurrentFOV);
}

// Called to bind functionality to input
void ACoopCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACoopCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACoopCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ACoopCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight", this, &ACoopCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACoopCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ACoopCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ACoopCharacter::ZoomIn);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ACoopCharacter::ZoomOut);
}

FVector ACoopCharacter::GetPawnViewLocation() const
{
	if (CameraComp) {
		return CameraComp->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}

