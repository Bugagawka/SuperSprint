// Fill out your copyright notice in the Description page of Project Settings.


#include "FallingPlatform.h"
#include "Components/SphereComponent.h"

// Sets default values
AFallingPlatform::AFallingPlatform()
{
	MySphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("My Sphere Component"));
	MySphereComponent->InitSphereRadius(100.f);
	MySphereComponent->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = MySphereComponent;

	MySphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AFallingPlatform::OnOverlapBegin);


	DeactivationDelay = 2.0f;
	ReactivationDelay = 5.0f;


}

void AFallingPlatform::DeactivateActor()
{
	// Deactivate the entire actor
	SetActorEnableCollision(false); // Disable collision for the actor
	SetActorHiddenInGame(true);    // Hide the actor in the game (optional)
}

void AFallingPlatform::ReactivateActor()
{
	// Reactivate the entire actor after the specified delay
	SetActorEnableCollision(true); // Enable collision for the actor
	SetActorHiddenInGame(false);   // Show the actor in the game (optional)
}

void AFallingPlatform::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		// Start a timer to deactivate the entire actor after a delay of 3 seconds
		FTimerHandle DeactivateTimerHandle;

		GetWorldTimerManager().SetTimer(DeactivateTimerHandle, this, &AFallingPlatform::DeactivateActor, DeactivationDelay, false);

		// Start another timer to reactivate the entire actor after an additional delay 
		FTimerHandle ReactivateTimerHandle;

		GetWorldTimerManager().SetTimer(ReactivateTimerHandle, this, &AFallingPlatform::ReactivateActor, ReactivationDelay, false);
	}
}



