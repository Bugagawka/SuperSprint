// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SuperSprintShowMovingPlatform.generated.h"

UCLASS()
class SUPERSPRINTSHOW_API ASuperSprintShowMovingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASuperSprintShowMovingPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathing", meta = (ExposeOnSpawn = true, MakeEditWidget = true))
	TArray<FVector> ThePath;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UBoxComponent* BoxCollider;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UStaticMeshComponent* PlatformMesh;


	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UInterpToMovementComponent* MovementComponent;
};
