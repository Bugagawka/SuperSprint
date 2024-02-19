 // Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"

#include "SuperSprintShowCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ASuperSprintShowCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Character movement speed increase **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float SpeedIncreaseRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MaxSpeed;

	// Crouching
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	/** Sliding Input Action **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlideAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Slide, meta = (AllowPrivateAccess = "true"))
	float SlideSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Slide, meta = (AllowPrivateAccess = "true"))
	float SlideFriction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Slide, meta = (AllowPrivateAccess = "true"))
	float NormalSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Slide, meta = (AllowPrivateAccess = "true"))
	float NormalFriction;

	UPROPERTY(ReplicatedUsing=OnRep_ReplicatedSpeed)
	float ReplicatedSpeed;

	UFUNCTION()
	void OnRep_ReplicatedSpeed();

public:
	ASuperSprintShowCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadWrite, Category = "Movement", Replicated)
	bool bIsSliding;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(Blueprintcallable)
	void StartSlide();

	UFUNCTION(Server, Reliable)
	void ServerStartSlide();

	UFUNCTION(Server, Reliable)
	void ServerEndSlide();

	UFUNCTION(Server, Reliable)
	void ServerIncreaseSpeed();

	UFUNCTION(Blueprintcallable)
	void EndSlide();

	UFUNCTION(Blueprintcallable)
	bool CanSlide();

private:
	void StartCrouch();
	void StopCrouch();

	float PreSlideSpeed;

	// Stores the original height of the capsule component
	float OriginalHeight;

};

