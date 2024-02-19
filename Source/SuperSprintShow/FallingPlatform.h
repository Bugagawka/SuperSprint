// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FallingPlatform.generated.h"

UCLASS()
class SUPERSPRINTSHOW_API AFallingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFallingPlatform();


public:

	// Declare the functions to deactivate and reactivate the actor
	void DeactivateActor();
	void ReactivateActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings")
	float DeactivationDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings")
	float ReactivationDelay;




	UPROPERTY(VisibleAnywhere)
	class USphereComponent* MySphereComponent;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
