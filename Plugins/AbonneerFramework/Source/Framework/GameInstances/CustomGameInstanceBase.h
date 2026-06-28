// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/AdvancedSessions/BlueprintDataDefinitions.h"
#include "CustomGameInstanceBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameInstance, Log, All);

/**
 * Custom Game Instance For The Abonneer Framework.
 * Handles things like multiplayer peer to peer hosting/joining/ending/leaving
 * Handles user settings
 * & more
 */
UCLASS()
class FRAMEWORK_API UCustomGameInstanceBase : public UGameInstance
{
	GENERATED_BODY()

public:
	UCustomGameInstanceBase(const FObjectInitializer& objectInitializer);

	virtual void Init() override;
	virtual void LoadComplete(const float loadTime, const FString& mapName) override;
	virtual void Shutdown() override;

private:

	//-------------------------------------------------------------
	//-----------------  GENERAL SETTINGS  -------------------------
	//-------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	void CustomReturnToMainMenu();
	int32 GetMaxNrOfPlayers();

	int32 ExpectedPlayers = 0;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> MainMenuLevel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxNrOfPlayers;


private:

	//-------------------------------------------------------------
	//-------------------  SESSIONS  ------------------------------
	//-------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	bool IsSessionHost() { return IsHost; }
	UFUNCTION(BlueprintCallable)
	void Debug_SetSessionHost(bool value) { IsHost = value; }

	virtual void HandleNetworkFailure(UWorld* world, UNetDriver* netDriver, ENetworkFailure::Type failureType, const FString& errorString);
	void StartNewParty(EOnlineSessionType sessionType, TFunction<void(bool)> onCompletedLambda = nullptr);
	void FindSessions(TFunction<void()> onCompletedLambda = nullptr);
	void JoinOnlineSession(TFunction<void()> onCompletedLambda = nullptr);
	void DestroySession(TFunction<void()> onCompletedLambda = nullptr);

	void JoinSessionByLobbyID(const FString& lobbyID, TFunction<void()> onCompletedLambda);
	void FindSessionByLobbyID(const FString& lobbyID, TSharedPtr<const FUniqueNetId> userId);
	void OnFindSessionByIDComplete(bool bWasSuccessful);
	FString GenerateUniqueLobbyID();
	FString GetSteamUserID();

	UFUNCTION(BlueprintCallable)
	void CreateAndEnterLobby(TSoftObjectPtr<UWorld> LevelToLoad, EOnlineSessionType OnlineSessionType);

	const TSharedPtr<class FOnlineSessionSearch> GetSessionSearchResults() { return SessionSearchResults; }

	bool JoinSessionAdvanced(const FOnlineSessionSearchResult& searchResult);
protected:
	bool HostSessionAdvanced(int32 publicConnections, int32 privateConnections, bool bUseLAN, bool bAllowInvites, bool bIsDedicatedServer/*, bool bUsePresence*/, bool bUseLobbiesIfAvailable, bool bAllowJoinViaPresence, bool bAllowJoinViaPresenceFriendsOnly, bool bAntiCheatProtected, bool bUsesStats, bool bShouldAdvertise, bool bUseLobbiesVoiceChatIfAvailable, const TArray<FSessionPropertyKeyPair>& ExtraSettings = TArray<FSessionPropertyKeyPair>());
	void FindSessionsAdvanced(TSharedPtr<const FUniqueNetId> userId, bool bIsLAN, bool bIsPresence);

	virtual void OnCreateSessionComplete(FName sessionName, bool bWasSuccessful);
	void OnStartOnlineGameComplete(FName sessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	virtual void OnDestroySessionComplete(FName sessionName, bool bWasSuccessful);

private:
	bool IsHost = false;
	bool IsInLobby = false;

	FString CurrentLobbyID;
	FString TargetLobbyID;

	/* Delegate called when session created */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/* Delegate called when session started */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	/** Handles to registered delegates for creating/starting a session */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	/** Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	/** Handle to registered delegate for searching a session */
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSearch> SessionSearchResults;

	/** Delegate for joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	/** Handle to registered delegate for joining a session */
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	/** Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	/** Handle to registered delegate for destroying a session */
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	FDelegateHandle OnFindSessionByIDCompleteDelegateHandle;

	TFunction<void(bool)> OnStartedDelegate;
	TFunction<void()> OnFoundDelegate;
	TFunction<void()> OnJoinedDelegate;
	TFunction<void()> OnDestroyedDelegate;

	//-------------------------------------------------------------
	//----------------  SESSION INVITES  --------------------------
	//-------------------------------------------------------------

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvancedFriendsInterface)
	bool bCallFriendInterfaceEventsOnPlayerControllers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvancedFriendsInterface)
	bool bCallIdentityInterfaceEventsOnPlayerControllers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvancedFriendsInterface)
	bool bCallVoiceInterfaceEventsOnPlayerControllers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvancedVoiceInterface)
	bool bEnableTalkingStatusDelegate;

	//*** Session invite received by local ***//
	FOnSessionInviteReceivedDelegate SessionInviteReceivedDelegate;
	FDelegateHandle SessionInviteReceivedDelegateHandle;

	// custom Steam UI Join User function #Self invite#
	void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 controllerId, FUniqueNetIdPtr userId, const FOnlineSessionSearchResult& inviteResult);
	//const FUniqueNetId& /*UserId*/, const FUniqueNetId& /*FromId*/, const FString& /*AppId*/, const FOnlineSessionSearchResult& /*InviteResult*/
	void OnSessionInviteReceivedMaster(const FUniqueNetId& personInvited, const FUniqueNetId& personInviting, const FString& appId, const FOnlineSessionSearchResult& sessionToJoin);
	// After a session invite has been accepted by the local player this event is triggered, call JoinSession on the session result to join it
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedFriends")
	void OnSessionInviteReceived(int32 localPlayerNum, FBPUniqueNetId personInviting, const FString& appId, const FBlueprintSessionResult& sessionToJoin);

	//*** Session invite accepted by local ***//
	FOnSessionUserInviteAcceptedDelegate SessionInviteAcceptedDelegate;
	FDelegateHandle SessionInviteAcceptedDelegateHandle;

	void OnSessionInviteAcceptedMaster(const bool bWasSuccessful, int32 localPlayer, TSharedPtr<const FUniqueNetId> personInviting, const FOnlineSessionSearchResult& sessionToJoin);

	// After a session invite has been accepted by the local player this event is triggered, call JoinSession on the session result to join it
	// This function is currently not hooked up in any of Epics default subsystems, it is here for custom subsystems
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedFriends")
	void OnSessionInviteAccepted(int32 localPlayerNum, FBPUniqueNetId personInvited, const FBlueprintSessionResult& sessionToJoin);


	// After a voice status has changed this event is triggered if the bEnableTalkingStatusDelegate property is true
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedVoice")
	void OnPlayerTalkingStateChanged(FBPUniqueNetId playerId, bool bIsTalking);
	void OnPlayerTalkingStateChangedMaster(TSharedRef<const FUniqueNetId> playerId, bool bIsTalking);

	FOnPlayerTalkingStateChangedDelegate PlayerTalkingStateChangedDelegate;
	FDelegateHandle PlayerTalkingStateChangedDelegateHandle;


	// Called when the designated LocalUser has changed login state
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedIdentity", meta = (DisplayName = "OnPlayerLoginChanged"))
	void OnPlayerLoginChanged(int32 playerNum);

	void OnPlayerLoginChangedMaster(int32 playerNum);
	FOnLoginChangedDelegate PlayerLoginChangedDelegate;
	FDelegateHandle PlayerLoginChangedDelegateHandle;

	// Called when the designated LocalUser has changed login status
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedIdentity", meta = (DisplayName = "OnPlayerLoginStatusChanged"))
	void OnPlayerLoginStatusChanged(int32 playerNum, EBPLoginStatus previousStatus, EBPLoginStatus newStatus, FBPUniqueNetId newPlayerUniqueNetID);

	void OnPlayerLoginStatusChangedMaster(int32 playerNum, ELoginStatus::Type previousStatus, ELoginStatus::Type newStatus, const FUniqueNetId& newPlayerUniqueNetID);
	FOnLoginStatusChangedDelegate PlayerLoginStatusChangedDelegate;
	FDelegateHandle PlayerLoginStatusChangedDelegateHandle;

	//-------------------------------------------------------------
	//-----------------  LOADING SCREEN  --------------------------
	//-------------------------------------------------------------
	protected:
	//	void OnSeamlessTravelStart(UWorld* InWorld);
	//	void OnSeamlessTravelComplete(UWorld* InWorld);	
	// 
	
		UPROPERTY(EditDefaultsOnly, Category = "Loading")
		TSubclassOf<UUserWidget> LoadingScreenWidgetClass;

		UFUNCTION(BlueprintCallable)
		void EnableLoadingScreens();
		UFUNCTION()
		virtual void BeginLoadingScreen(const FString& MapName);
		bool bDelegatesBound = false;
		UFUNCTION()
		virtual void EndLoadingScreen(UWorld* InLoadedWorld);

};
