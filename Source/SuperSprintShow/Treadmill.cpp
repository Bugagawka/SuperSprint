// Fill out your copyright notice in the Description page of Project Settings.


#include "Treadmill.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

// Sets default values
ATreadmill::ATreadmill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ATreadmill::OnPlayerOverlapBegin);
    TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ATreadmill::OnPlayerOverlapEnd);
    RootComponent = TriggerVolume;

    BoostMultiplier = 2.0f; // Set the default boost multiplier
    SlowSpeed = 50.0f; // Set the default slow speed
    OverlappingCharacter = nullptr;
    bIsMoving = false;
}

void ATreadmill::OnPlayerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ACharacter* Character = Cast<ACharacter>(OtherActor);
    if (Character)
    {
        OverlappingCharacter = Character;
        GetWorldTimerManager().SetTimer(MovementTimerHandle, this, &ATreadmill::MoveCharacterForward, 0.1f, true);

        // Apply boost only when starting to move on the treadmill
        Character->GetCharacterMovement()->MaxWalkSpeed *= BoostMultiplier;
    }
}

void ATreadmill::OnPlayerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor == OverlappingCharacter && OverlappingCharacter != nullptr)
    {
        GetWorldTimerManager().ClearTimer(MovementTimerHandle);
        bIsMoving = false;

        if (OverlappingCharacter->GetCharacterMovement() != nullptr)
        {
            OverlappingCharacter->GetCharacterMovement()->MaxWalkSpeed /= BoostMultiplier;
        }

        OverlappingCharacter = nullptr;
    }
}

void ATreadmill::MoveCharacterForward()
{
    if (true)
    {
        if (OverlappingCharacter)
        {
            FVector CurrentVelocity = OverlappingCharacter->GetCharacterMovement()->Velocity;
            float ForwardVelocity = FVector::DotProduct(CurrentVelocity, OverlappingCharacter->GetActorForwardVector());

            if (ForwardVelocity < 1.0f) // If the character is moving slowly or standing still
            {
                FVector NewLocation = OverlappingCharacter->GetActorLocation() + (OverlappingCharacter->GetActorForwardVector() * SlowSpeed * GetWorld()->GetDeltaSeconds());
                OverlappingCharacter->SetActorLocation(NewLocation);
                bIsMoving = true; // Character is moving or standing still
            }
            else
            {
                bIsMoving = false; // Character is moving at a higher speed
            }
        }
    }
 
}

