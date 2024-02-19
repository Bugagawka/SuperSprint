// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticBaseAndRotator.h"

// Sets default values
AStaticBaseAndRotator::AStaticBaseAndRotator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create components
	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RotatingObjectMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RotatingObjectMesh"));

	// Attach rotating object to the base
	RotatingObjectMeshComponent->SetupAttachment(BaseMeshComponent);

	Speed = 10.0f;
}

// Called when the game starts or when spawned
void AStaticBaseAndRotator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStaticBaseAndRotator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (true)
	{
		// Rotate the base mesh
		//FRotator NewRotation = BaseMeshComponent->GetComponentRotation();
		//NewRotation.Yaw += DeltaTime * Speed; // Adjust the rotation speed as needed
		//BaseMeshComponent->SetWorldRotation(NewRotation);
		 // Rotate the rotating object mesh
		FRotator NewRotation = RotatingObjectMeshComponent->GetComponentRotation();
		NewRotation.Yaw += DeltaTime * Speed; // Adjust the rotation speed as needed
		RotatingObjectMeshComponent->SetWorldRotation(NewRotation);
	}
}

