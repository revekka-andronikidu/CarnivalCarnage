// Copyright Abonneer, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Online/AdvancedSessions/BlueprintDataDefinitions.h"
#include "MainPlayerState.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct FLeaderboardEntry
{
    GENERATED_BODY()

    UPROPERTY() FString PlayerName;

    UPROPERTY() int32 PlayerScore;

    UPROPERTY()
    UTexture2D* AvatarTexture;

    UPROPERTY()	float Color;
};

class ALobbyCharacterActor;

UCLASS()
class STARTERPROJECT_API AMainPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
    TSubclassOf<ALobbyCharacterActor> SelectedCharacterClass;

    FLeaderboardEntry GetLeaderboardEntry() const;

    void SetSteamPlayerName(const FString& NewName);
   
    UFUNCTION(BlueprintCallable) const FString GetSteamPlayerName() const { return SteamPlayerName; };
    UTexture2D* GetSteamAvatar() const;

    UFUNCTION()
    void OnRep_CharacterColor();

    void SetColor(float color);
    float GetColor() { return Color; };

    UFUNCTION()
    void OnRep_SelectedCharacterClass();

    void SetSelectedCharacter(TSubclassOf<ALobbyCharacterActor> InClass);

    UFUNCTION() void AddWin();
    UFUNCTION(BlueprintCallable) int32 GetNumberOfWins()  const { return numberOfWins; };

protected:
    virtual void SeamlessTravelTo(APlayerState* NewPlayerState) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
    float Color;

    UPROPERTY(ReplicatedUsing = OnRep_PlayerName)
    FString SteamPlayerName;

    /*UPROPERTY(ReplicatedUsing = OnRep_SteamAvatar)
    UTexture2D* SteamAvatar;*/
	
private:
    UPROPERTY(ReplicatedUsing = OnRep_RoundWon)
    int32 numberOfWins = 0;


    UFUNCTION()
    void OnRep_RoundWon();

    
};
