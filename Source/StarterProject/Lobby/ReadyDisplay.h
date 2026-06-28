// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReadyDisplay.generated.h"

class UScrollTextWidget;

UCLASS()
class STARTERPROJECT_API AReadyDisplay : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReadyDisplay();
	void SetReadyState(bool bReady);
    
    void SetPlayerName(FString playerName);
    void ResetSign();

    virtual void Tick(float DeltaSeconds) override;

protected:

	virtual void BeginPlay() override;

    UFUNCTION(BlueprintImplementableEvent)
    void ReadyStatusChangedEvent(bool isReady);
    // To store if the actor is ready
    UPROPERTY(ReplicatedUsing = OnRep_IsReady, BlueprintReadOnly)
    bool IsReady = false;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Lightbulbs")
    UStaticMeshComponent* TargetMesh;

    // Materials for ready/unready states
    UPROPERTY(EditAnywhere, Category = "Lightbulbs")
    UMaterialInterface* LightMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightbulbs")
    FName IntensityParameterName = TEXT("Intensity");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightbulbs")
    float ReadyIntensity = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightbulbs")
    float NotReadyIntensity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightbulbs")
    FLinearColor StartColor = FLinearColor::White;  // Default white

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightbulbs")
    FName ColorParameterName = TEXT("Color");


    UPROPERTY()  UMaterialInstanceDynamic* DynamicMaterial;

    UPROPERTY()   UScrollTextWidget* TextWidget;

    UPROPERTY(ReplicatedUsing = OnRep_PlayerText)
    FString PlayerText;

    UFUNCTION()  void OnRep_IsReady();
    UFUNCTION()  void OnRep_PlayerText();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

};


