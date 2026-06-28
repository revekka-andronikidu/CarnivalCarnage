#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThirdPersonCharacter.generated.h"

struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class USmoothSync;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogThirdPersonCharacter, Log, All);

/**
 * A basic third person character, with third person camera & movement
*/
UCLASS()
class FRAMEWORK_API AThirdPersonCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	//**************************
	//******* Default **********
	//**************************
	// Sets default values for this character's properties
	AThirdPersonCharacter(const FObjectInitializer& objectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* inputComponent) override;

	virtual void PossessedBy(AController* newController) override;
	virtual void OnPlayerStateChanged(APlayerState* newPlayerState, APlayerState* oldPlayerState) override;

	//*****************************************
	//***********  Components *****************
	//*****************************************
	/** Third person Spring Arm */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;
	/** Third person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* ThirdPersonCamera;
	/** Smooth Sync For Replication */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	USmoothSync* SmoothSync;

	//*****************************************
	//************  Sprinting  ****************
	//*****************************************
	/** Wether Or Not Sprinting Is Enabled */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsSprintingEnabled;
	// Cannot be changed directly, should be changed in movement component instead
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Runtime|Movement", meta = (AllowPrivateAccess = "true"))
	float WalkingMovementSpeed;
	// Movement speed used when sprinting
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Movement", meta = (AllowPrivateAccess = "true"))
	float SprintingMovementSpeed;
	
	//*****************************************
	//*************  Camera  ******************
	//*****************************************
	// A local pitch variable, not directly through controller
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Runtime|Camera", meta = (AllowPrivateAccess = "true"))
	float ControlledPitch;
	/** The maximum angle the player is allowed to look up/down */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Camera", meta = (AllowPrivateAccess = "true"))
	float MaxPitchAngle;
	/** The Min and Max Spring Arm Length To Use In Third Person (based on distance) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Camera", meta = (AllowPrivateAccess = "true"))
	FVector2D MinMaxSpringArmLength;
	
	//*****************************************
	//*************  INPUT  *******************
	//*****************************************
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	/** Sprinting Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintingAction;
	/** Crouching Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchingAction;

	/** Called for movement input */
	virtual void Move(const FInputActionValue& value);
	/** Called for looking input */
	virtual void Look(const FInputActionValue& value);
	/** Called for jumping input */
	virtual void StartJump();
	/** Called for sprinting input */
	virtual void StartSprinting();
	virtual void StopSprinting();
	/** Called for crouching input */
	virtual void StartCrouch();
	virtual void StopCrouch();

	virtual void OnEndCrouch(float halfHeightAdjust, float scaledHalfHeightAdjust) override;
	virtual void OnStartCrouch(float halfHeightAdjust, float scaledHalfHeightAdjust) override;

private:
};