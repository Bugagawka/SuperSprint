// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StaticBaseAndRotator.generated.h"

UCLASS()
class SUPERSPRINTSHOW_API AStaticBaseAndRotator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStaticBaseAndRotator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	float Speed;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RotatingObjectMeshComponent;

};
