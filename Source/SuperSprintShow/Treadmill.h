// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Treadmill.generated.h"

UCLASS()
class SUPERSPRINTSHOW_API ATreadmill : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATreadmill();


    UPROPERTY(EditAnywhere)
    UBoxComponent* TriggerVolume;

    UPROPERTY(EditAnywhere)
    float BoostMultiplier; // Multiplier for character's movement speed boost

    UPROPERTY(EditAnywhere)
    float SlowSpeed; // Speed at which the character moves slowly

    UFUNCTION()
    void OnPlayerOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnPlayerOverlapEnd(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex
    );

    void MoveCharacterForward(); // Function to move the character forward slowly
   
    ACharacter* OverlappingCharacter; // Reference to the character on the treadmill
    bool bIsMoving; // Flag indicating if the character is moving
    FTimerHandle MovementTimerHandle; // Timer handle for character movement



};
