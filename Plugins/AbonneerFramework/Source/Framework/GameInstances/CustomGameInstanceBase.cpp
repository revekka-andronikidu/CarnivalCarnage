#include "CustomGameInstanceBase.h"

// Sounds
#include "Sound/SoundSubmix.h"

//Loading Screen
#include "Blueprint/UserWidget.h"

// Utils
#include "Kismet/GameplayStatics.h"

// Online
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Online/AdvancedSessions/AdvancedFriendsInterface.h"

// Default
#include "GameFramework/GameState.h"

//	Loading screen
#include "Runtime/MoviePlayer/Public/MoviePlayer.h"


#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h" 
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h" 
#endif

DEFINE_LOG_CATEGORY(LogGameInstance);

UCustomGameInstanceBase::UCustomGameInstanceBase(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
	, bCallFriendInterfaceEventsOnPlayerControllers(true)
	, bCallIdentityInterfaceEventsOnPlayerControllers(true)
	, bCallVoiceInterfaceEventsOnPlayerControllers(true)
	, bEnableTalkingStatusDelegate(true)
	, SessionInviteReceivedDelegate(FOnSessionInviteReceivedDelegate::CreateUObject(this, &ThisClass::OnSessionInviteReceivedMaster))
	, SessionInviteAcceptedDelegate(FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &ThisClass::OnSessionInviteAcceptedMaster))
	, PlayerTalkingStateChangedDelegate(FOnPlayerTalkingStateChangedDelegate::CreateUObject(this, &ThisClass::OnPlayerTalkingStateChangedMaster))
	, PlayerLoginChangedDelegate(FOnLoginChangedDelegate::CreateUObject(this, &ThisClass::OnPlayerLoginChangedMaster))
	, PlayerLoginStatusChangedDelegate(FOnLoginStatusChangedDelegate::CreateUObject(this, &ThisClass::OnPlayerLoginStatusChangedMaster))
{
	/** Bind function for CREATING a Session */
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UCustomGameInstanceBase::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UCustomGameInstanceBase::OnStartOnlineGameComplete);

	/** Bind function for FINDING a Session */
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UCustomGameInstanceBase::OnFindSessionsComplete);

	/** Bind function for JOINING a Session */
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UCustomGameInstanceBase::OnJoinSessionComplete);

	/** Bind function for DESTROYING a Session */
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UCustomGameInstanceBase::OnDestroySessionComplete);

	MaxNrOfPlayers = 4;
}
void UCustomGameInstanceBase::Init()
{
	Super::Init();

	IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());//OnlineSub->GetSessionInterface();

	if (sessionInterface.IsValid())
	{
		// Currently doesn't store a handle or assign a delegate to any local player beyond the first.....should handle?
		// Thought about directly handling it but friends for multiple players probably isn't required
		// Iterating through the local player TArray only works if it has had players assigned to it, most of the online interfaces don't support
		// Multiple logins either (IE: Steam)
		SessionInviteAcceptedDelegateHandle = sessionInterface->AddOnSessionUserInviteAcceptedDelegate_Handle(SessionInviteAcceptedDelegate);

		SessionInviteReceivedDelegateHandle = sessionInterface->AddOnSessionInviteReceivedDelegate_Handle(SessionInviteReceivedDelegate);

		// Custom steam join game delegate
		sessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &UCustomGameInstanceBase::OnSessionUserInviteAccepted);
	}
	else
	{
		UE_LOG(LogGameInstance, Warning, TEXT("[UCustomGameInstanceBase] Failed to get session interface!"));
		//return;
	}

	// Beginning work on the voice interface
	if (bEnableTalkingStatusDelegate)
	{
		IOnlineVoicePtr voiceInterface = Online::GetVoiceInterface(GetWorld());

		if (voiceInterface.IsValid())
		{
			PlayerTalkingStateChangedDelegateHandle = voiceInterface->AddOnPlayerTalkingStateChangedDelegate_Handle(PlayerTalkingStateChangedDelegate);
		}
		else
		{

			UE_LOG(LogGameInstance, Warning, TEXT("[UCustomGameInstanceBase] Failed to get voice interface!"));
		}
	}

	IOnlineIdentityPtr identityInterface = Online::GetIdentityInterface(GetWorld());

	if (identityInterface.IsValid())
	{
		PlayerLoginChangedDelegateHandle = identityInterface->AddOnLoginChangedDelegate_Handle(PlayerLoginChangedDelegate);

		// Just defaulting to player 1
		PlayerLoginStatusChangedDelegateHandle = identityInterface->AddOnLoginStatusChangedDelegate_Handle(0, PlayerLoginStatusChangedDelegate);
	}
	else
	{
		UE_LOG(LogGameInstance, Warning, TEXT("[UCustomGameInstanceBase] Failed to get identity interface!"));
	}

	

	GetEngine()->OnNetworkFailure().AddUObject(this, &UCustomGameInstanceBase::HandleNetworkFailure);
}

void UCustomGameInstanceBase::EnableLoadingScreens()
{
	if (bDelegatesBound)
		return;

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UCustomGameInstanceBase::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UCustomGameInstanceBase::EndLoadingScreen);

	bDelegatesBound = true;
}
void UCustomGameInstanceBase::LoadComplete(const float loadTime, const FString& mapName)
{
	Super::LoadComplete(loadTime, mapName);

	UE_LOG(LogGameInstance, Display, TEXT("[UCustomGameInstanceBase] Loaded Map '%s' in %f seconds!"), *mapName, loadTime);
}
void UCustomGameInstanceBase::Shutdown()
{
	DestroySession([] {
		UE_LOG(LogGameInstance, Display, TEXT("[UCustomGameInstanceBase] Shutdown detected, destroyed session."));
		});


	IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());

	if (!sessionInterface.IsValid())
	{
		UE_LOG(LogGameInstance, Warning, TEXT("UAdvancedFriendsGameInstance Failed to get session system!"));
		//return;
	}
	else
	{
		// Clear all of the delegate handles here
		sessionInterface->ClearOnSessionUserInviteAcceptedDelegate_Handle(SessionInviteAcceptedDelegateHandle);
		sessionInterface->ClearOnSessionInviteReceivedDelegate_Handle(SessionInviteReceivedDelegateHandle);
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
	}


	if (bEnableTalkingStatusDelegate)
	{
		IOnlineVoicePtr voiceInterface = Online::GetVoiceInterface(GetWorld());

		if (voiceInterface.IsValid())
		{
			voiceInterface->ClearOnPlayerTalkingStateChangedDelegate_Handle(PlayerTalkingStateChangedDelegateHandle);
		}
		else
		{

			UE_LOG(LogGameInstance, Warning, TEXT("[UCustomGameInstanceBase] Failed to get voice interface!"));
		}
	}

	IOnlineIdentityPtr identityInterface = Online::GetIdentityInterface(GetWorld());

	if (identityInterface.IsValid())
	{
		identityInterface->ClearOnLoginChangedDelegate_Handle(PlayerLoginChangedDelegateHandle);


		// I am just defaulting to player 1
		identityInterface->ClearOnLoginStatusChangedDelegate_Handle(0, PlayerLoginStatusChangedDelegateHandle);
	}


	Super::Shutdown();
}

//-------------------------------------------------------------
//--------------------  GENERAL  ------------------------------
//-------------------------------------------------------------
void UCustomGameInstanceBase::CustomReturnToMainMenu()
{
	DestroySession([] {
		UE_LOG(LogGameInstance, Display, TEXT("[UCustomGameInstanceBase] Returning to main menu, destroyed session."));
		});

	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), MainMenuLevel, true);
}
int32 UCustomGameInstanceBase::GetMaxNrOfPlayers()
{
	return MaxNrOfPlayers;
}

//-------------------------------------------------------------
//-------------------  SESSIONS  ------------------------------
//-------------------------------------------------------------
void UCustomGameInstanceBase::StartNewParty(EOnlineSessionType sessionType, TFunction<void(bool)> onCompletedLambda)
{
	OnStartedDelegate = onCompletedLambda;

	bool allowJoinViaPresence = true;
	bool allowJoinViaPresenceFriendsOnly = true;
	bool bCanFindInServerBrowser = true;
	switch (sessionType)
	{
	case EOnlineSessionType::Public:
		allowJoinViaPresence = true;
		allowJoinViaPresenceFriendsOnly = false;
		bCanFindInServerBrowser = true;
		break;
	case EOnlineSessionType::FriendsOnly:
		allowJoinViaPresence = true;
		allowJoinViaPresenceFriendsOnly = true;
		bCanFindInServerBrowser = false;
		break;
	case EOnlineSessionType::InviteOnly:
		allowJoinViaPresence = false;
		allowJoinViaPresenceFriendsOnly = false;
		bCanFindInServerBrowser = false;
		break;
	default:
		break;
	}

	TArray<FSessionPropertyKeyPair> extraSettings;

	// Extra Settings
	CurrentLobbyID = GenerateUniqueLobbyID();
	extraSettings.Add(FSessionPropertyKeyPair(TEXT("isInviteOnly"), static_cast<int32>((!bCanFindInServerBrowser) ? 1 : 0)));
	extraSettings.Add(FSessionPropertyKeyPair(TEXT("LobbyID"), CurrentLobbyID));

#if PLATFORM_WINDOWS
	int geoId = GetUserGeoID(16);
	int lcid = GetUserDefaultLCID();
	wchar_t locationBuffer[3];
	GetGeoInfo(geoId, 4, locationBuffer, 3, lcid);

	extraSettings.Add(FSessionPropertyKeyPair(TEXT("RegionCode"), locationBuffer));
#elif
	UE_LOG(LogGameInstance, Warning, TEXT("Game is not running on windows, cannot get user country from Windows platform."));
#endif

	// Call our custom HostSession function. GameSessionName is a GameInstance variable
	if (!HostSessionAdvanced(
		MaxNrOfPlayers, // public connections
		0, // private connections
		false, // useLAN
		true, // AllowInvites
		false, // IsDedicatedServer
		//true, // UsePresence
		true, // Use Lobbies
		allowJoinViaPresence, // Allow join via presence
		allowJoinViaPresenceFriendsOnly, // Allow join via presence friends only
		false, // AntiCheatProtected
		false, // UsesStats
		true, // ShouldAdvertise
		false, // UseLobbiesVoiceChat
		extraSettings)) // Extra Settings
	{
		if (OnStartedDelegate)
		{
			OnStartedDelegate(false);
		}
	}
}
bool UCustomGameInstanceBase::HostSessionAdvanced(int32 publicConnections, int32 privateConnections, bool bUseLAN, bool bAllowInvites, bool bIsDedicatedServer/*, bool bUsePresence*/, bool bUseLobbiesIfAvailable, bool bAllowJoinViaPresence, bool bAllowJoinViaPresenceFriendsOnly, bool bAntiCheatProtected, bool bUsesStats, bool bShouldAdvertise, bool bUseLobbiesVoiceChatIfAvailable, const TArray<FSessionPropertyKeyPair>& extraSettings)
{
	FOnlineSubsystemBPCallHelperAdvanced helper(TEXT("CreateSession"), GEngine->GetWorldFromContextObject(GetWorld(), EGetWorldErrorMode::LogAndReturnNull));

	auto playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (playerController)
		helper.QueryIDFromPlayerController(playerController);

	if (helper.OnlineSub != nullptr)
	{
		auto sessions = helper.OnlineSub->GetSessionInterface();
		if (sessions.IsValid())
		{
			OnCreateSessionCompleteDelegateHandle = sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

			FOnlineSessionSettings settings;
			settings.NumPublicConnections = publicConnections;
			settings.NumPrivateConnections = privateConnections;
			settings.bShouldAdvertise = bShouldAdvertise;
			settings.bAllowJoinInProgress = true;
			settings.bIsLANMatch = bUseLAN;
			settings.bAllowJoinViaPresence = bAllowJoinViaPresence;
			settings.bIsDedicated = bIsDedicatedServer;
			settings.bUseLobbiesVoiceChatIfAvailable = bUseLobbiesIfAvailable ? bUseLobbiesVoiceChatIfAvailable : false;
			settings.bAllowJoinViaPresenceFriendsOnly = bAllowJoinViaPresenceFriendsOnly;
			settings.bAntiCheatProtected = bAntiCheatProtected;
			settings.bUsesStats = bUsesStats;

			if (bIsDedicatedServer)
			{
				settings.bUsesPresence = false;
				settings.bUseLobbiesIfAvailable = false;
			}
			else
			{
				settings.bUsesPresence = true;
				settings.bUseLobbiesIfAvailable = true;
			}

			// These are about the only changes over the standard Create Sessions Node
			settings.bAllowInvites = bAllowInvites;

			FOnlineSessionSetting extraSetting;
			for (int i = 0; i < extraSettings.Num(); i++)
			{
				extraSetting.Data = extraSettings[i].Data;
				extraSetting.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineServiceAndPing;
				settings.Set(extraSettings[i].Key, extraSetting);
			}

			if (!bIsDedicatedServer)
			{
				if (playerController && helper.UserID.IsValid())
				{
					sessions->CreateSession(*helper.UserID, NAME_GameSession, settings);
				}
				else
				{
					FFrame::KismetExecutionMessage(TEXT("Invalid Player controller when attempting to start a session"), ELogVerbosity::Warning);
					sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
				}
			}
			else
			{
				sessions->CreateSession(0, NAME_GameSession, settings);
			}

			// OnCreateCompleted will get called, nothing more to do now
			return true;
		}
	}

	return false;
}
void UCustomGameInstanceBase::JoinOnlineSession(TFunction<void()> onCompletedLambda)
{
	OnJoinedDelegate = onCompletedLambda;

	ULocalPlayer* const player = GetFirstGamePlayer();

	// Just a SearchResult where we can save the one we want to use, for the case we find more than one!
	FOnlineSessionSearchResult searchResult;

	// If the Array is not empty, we can go through it
	if (SessionSearchResults->SearchResults.Num() > 0)
	{
		for (int32 i = 0; i < SessionSearchResults->SearchResults.Num(); i++)
		{
			// To avoid something crazy, we filter sessions from ourself
			if (SessionSearchResults->SearchResults[i].Session.OwningUserId != player->GetPreferredUniqueNetId().GetUniqueNetId())
			{
				searchResult = SessionSearchResults->SearchResults[i];

				// Once we found sounce a Session that is not ours, just join it. Instead of using a for loop, you could
				// use a widget where you click on and have a reference for the GameSession it represents which you can use
				// here
				JoinSessionAdvanced(searchResult);
				break;
			}
		}
	}
}
bool UCustomGameInstanceBase::JoinSessionAdvanced(const FOnlineSessionSearchResult& searchResult)
{
	// Return bool
	bool bSuccessful = false;

	FOnlineSubsystemBPCallHelperAdvanced helper(TEXT("CreateSession"), GEngine->GetWorldFromContextObject(GetWorld(), EGetWorldErrorMode::LogAndReturnNull));

	auto playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (playerController)
		helper.QueryIDFromPlayerController(playerController);

	if (helper.OnlineSub != nullptr)
	{
		auto sessions = helper.OnlineSub->GetSessionInterface();
		if (sessions.IsValid())
		{
			// Set the Handle again
			OnJoinSessionCompleteDelegateHandle = sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
			// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
			bSuccessful = sessions->JoinSession(*helper.UserID, NAME_GameSession, searchResult);
		}
	}
	return bSuccessful;
}
void UCustomGameInstanceBase::DestroySession(TFunction<void()> onCompletedLambda)
{
	OnDestroyedDelegate = onCompletedLambda;

	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();
	if (onlineSub)
	{
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();


		if (sessions.IsValid())
		{
			sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
			sessions->DestroySession(NAME_GameSession);
		}
	}
}
void UCustomGameInstanceBase::FindSessions(TFunction<void()> onCompletedLambda)
{
	OnFoundDelegate = onCompletedLambda;

	ULocalPlayer* const player = GetFirstGamePlayer();
	FindSessionsAdvanced(player->GetPreferredUniqueNetId().GetUniqueNetId(), false, true);
}
void UCustomGameInstanceBase::FindSessionsAdvanced(TSharedPtr<const FUniqueNetId> userId, bool bIsLAN, bool bIsPresence)
{
	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();

	if (onlineSub)
	{
		// Get the SessionInterface from our OnlineSubsystem
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid() && userId.IsValid())
		{
			/*
				Fill in all the SearchSettings, like if we are searching for a LAN game and how many results we want to have!
			*/
			SessionSearchResults = MakeShareable(new FOnlineSessionSearch());

			// Increase search pool for better matchmaking
			SessionSearchResults->MaxSearchResults = 50;
			SessionSearchResults->PingBucketSize = 50;
			SessionSearchResults->bIsLanQuery = bIsLAN;

			//// Enable Matchmaking using Steam Lobbies
			SessionSearchResults->QuerySettings.Set(SEARCH_LOBBIES, bIsPresence, EOnlineComparisonOp::Equals);

			// Only Not Invite Only
			SessionSearchResults->QuerySettings.Set(TEXT("isInviteOnly"), 0, EOnlineComparisonOp::Equals);

			// Custom matchmaking criteria (Optional)
			//SessionSearchResults->QuerySettings.Set(FName(TEXT("SkillRating")), 1500, EOnlineComparisonOp::Near); // Example skill-based match
			//SessionSearchResults->QuerySettings.Set(FName(TEXT("Region")), FString("EU"), EOnlineComparisonOp::Equals); // Example region-based matchmaking

			TSharedRef<FOnlineSessionSearch> searchSettingsRef = SessionSearchResults.ToSharedRef();

			// Set the Delegate to the Delegate Handle of the FindSession function
			OnFindSessionsCompleteDelegateHandle = sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

			// Finally call the SessionInterface function. The Delegate gets called once this is finished
			sessions->FindSessions(*userId, searchSettingsRef);
		}
	}
	else
	{
		// If something goes wrong, just call the Delegate Function directly with "false".
		OnFindSessionsComplete(false);
	}
}

void UCustomGameInstanceBase::JoinSessionByLobbyID(const FString& lobbyID, TFunction<void()> onCompletedLambda)
{
	OnJoinedDelegate = onCompletedLambda;

	ULocalPlayer* const player = GetFirstGamePlayer();
	if (!player)
	{
		UE_LOG(LogGameInstance, Error, TEXT("No valid player controller found"));
		return;
	}

	// First, we need to find sessions with the specific lobby ID
	FindSessionByLobbyID(lobbyID, player->GetPreferredUniqueNetId().GetUniqueNetId());
}
void UCustomGameInstanceBase::FindSessionByLobbyID(const FString& lobbyID, TSharedPtr<const FUniqueNetId> userId)
{
	TargetLobbyID = lobbyID;

	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();
	if (onlineSub)
	{
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();
		if (sessions.IsValid() && userId.IsValid())
		{
			SessionSearchResults = MakeShareable(new FOnlineSessionSearch());
			SessionSearchResults->MaxSearchResults = 50;
			SessionSearchResults->PingBucketSize = 50;
			SessionSearchResults->bIsLanQuery = false;

			// Enable Steam Lobbies search
			SessionSearchResults->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

			// Search for the specific lobby ID
			SessionSearchResults->QuerySettings.Set(TEXT("LobbyID"), lobbyID, EOnlineComparisonOp::Equals);

			TSharedRef<FOnlineSessionSearch> searchSettingsRef = SessionSearchResults.ToSharedRef();

			// Set up delegate for lobby ID search
			OnFindSessionByIDCompleteDelegateHandle = sessions->AddOnFindSessionsCompleteDelegate_Handle(
				FOnFindSessionsCompleteDelegate::CreateUObject(this, &UCustomGameInstanceBase::OnFindSessionByIDComplete)
			);

			sessions->FindSessions(*userId, searchSettingsRef);
		}
	}
	else
	{
		OnFindSessionByIDComplete(false);
	}
}
void UCustomGameInstanceBase::OnFindSessionByIDComplete(bool bWasSuccessful)
{
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();
	if (onlineSub)
	{
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();
		if (sessions.IsValid())
		{
			sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionByIDCompleteDelegateHandle);
		}
	}

	if (bWasSuccessful && SessionSearchResults.IsValid())
	{
		// Look for the session with matching lobby ID
		for (const FOnlineSessionSearchResult& result : SessionSearchResults->SearchResults)
		{
			FString foundLobbyID;
			if (result.Session.SessionSettings.Get(TEXT("LobbyID"), foundLobbyID))
			{
				if (foundLobbyID == TargetLobbyID)
				{
					// Found the target session, join it
					if (JoinSessionAdvanced(result))
					{
						UE_LOG(LogGameInstance, Log, TEXT("Successfully initiated join to lobby: %s"), *TargetLobbyID);
						return;
					}
				}
			}
		}
	}
}
FString UCustomGameInstanceBase::GenerateUniqueLobbyID()
{
	// Get Steam ID for global uniqueness (if available)
	FString steamID = GetSteamUserID();

	// Create timestamp in milliseconds for better precision
	int64 timestampMs = FDateTime::Now().ToUnixTimestamp() * 1000 + FDateTime::Now().GetMillisecond();

	// Add random component for extra collision prevention
	int32 randomNum = FMath::RandRange(0, 0xFFFF); // 16-bit random

	// Format: Last 4 digits of SteamID + Timestamp(hex) + Random(hex)
	FString steamSuffix = steamID.Right(4);
	FString timestampHex = FString::Printf(TEXT("%X"), (uint32)(timestampMs & 0xFFFFFFFF));
	FString randomHex = FString::Printf(TEXT("%X"), randomNum);

	// Result: e.g., "1234-A1B2C3D4-E5F6" (about 15 characters)
	FString lobbyId = FString::Printf(TEXT("%s-%s-%s"), *steamSuffix, *timestampHex, *randomHex);
	UE_LOG(LogGameInstance, Display, TEXT("Generated lobby with lobby id: %s"), *lobbyId);
	return lobbyId;
}
FString UCustomGameInstanceBase::GetSteamUserID()
{
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();
	if (onlineSub && onlineSub->GetSubsystemName() == "Steam")
	{
		IOnlineIdentityPtr identity = onlineSub->GetIdentityInterface();
		if (identity.IsValid())
		{
			ULocalPlayer* localPlayer = GetFirstGamePlayer();
			if (localPlayer)
			{
				TSharedPtr<const FUniqueNetId> userId = localPlayer->GetPreferredUniqueNetId().GetUniqueNetId();
				if (userId.IsValid())
				{
					// Convert Steam ID to string and extract numeric part
					FString fullSteamId = userId->ToString();

					// Steam IDs are in format "Steam:76561198..." - extract the numeric part
					FString numericPart;
					if (fullSteamId.StartsWith(TEXT("Steam:")))
					{
						numericPart = fullSteamId.RightChop(6); // Remove "Steam:" prefix
					}
					else
					{
						numericPart = fullSteamId;
					}

					return numericPart;
				}
			}
		}
	}
	return TEXT("");
}

void UCustomGameInstanceBase::OnCreateSessionComplete(FName sessionName, bool bWasSuccessful)
{

	// Get the OnlineSubsystem so we can get the Session Interface
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();
	if (onlineSub)
	{
		// Get the Session Interface to call the StartSession function
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid())
		{
			// Clear the SessionComplete delegate handle, since we finished this call
			sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("[UCustomGameInstanceBase] Created Session!")));

				// Set the StartSession delegate handle
				OnStartSessionCompleteDelegateHandle = sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

				// Our StartSessionComplete delegate should get called after this
				sessions->StartSession(sessionName);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("[UCustomGameInstanceBase] Failed To Create Session!")));
			}
		}
	}
}
void UCustomGameInstanceBase::OnStartOnlineGameComplete(FName sessionName, bool bWasSuccessful)
{
	// Get the Online Subsystem so we can get the Session Interface
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();
	if (onlineSub)
	{
		// Get the Session Interface to clear the Delegate
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();
		if (sessions.IsValid())
		{
			// Clear the delegate, since we are done with this call
			sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}

	if (bWasSuccessful)
	{
		IsHost = true;
		IsInLobby = true;
	}
	if (OnStartedDelegate)
	{
		OnStartedDelegate(bWasSuccessful);
	}
}
void UCustomGameInstanceBase::OnFindSessionsComplete(bool bWasSuccessful)
{
	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* const onlineSub = IOnlineSubsystem::Get();
	if (onlineSub)
	{
		// Get SessionInterface of the OnlineSubsystem
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();
		if (sessions.IsValid())
		{
			// Clear the Delegate handle, since we finished this call
			sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
		}
	}

	if (bWasSuccessful)
	{
		if (OnFoundDelegate)
		{
			// Call delegate
			OnFoundDelegate();
		}
	}
}
void UCustomGameInstanceBase::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface.IsValid())
	{
		FString connectInfo;
		if (sessionInterface->GetResolvedConnectString(NAME_GameSession, connectInfo))
		{
			APlayerController* playerController = GetFirstLocalPlayerController();
			if (playerController)
			{
				playerController->ClientTravel(connectInfo, ETravelType::TRAVEL_Absolute);
			}
		}
	}

	if (result == EOnJoinSessionCompleteResult::Success)
	{
		IsHost = false;
		IsInLobby = true;

		if (OnJoinedDelegate)
		{
			OnJoinedDelegate();
		}
	}
}
void UCustomGameInstanceBase::OnDestroySessionComplete(FName sessionName, bool bWasSuccessful)
{
	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();
	if (onlineSub)
	{
		// Get the SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid())
		{
			// Clear the Delegate
			sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		}
	}

	// If it was successful, we just load another level (could be a MainMenu!)
	if (bWasSuccessful)
	{
		IsInLobby = false;
		IsHost = false;
		if (OnDestroyedDelegate)
			OnDestroyedDelegate();
	}
}

void UCustomGameInstanceBase::HandleNetworkFailure(UWorld* world, UNetDriver* netDriver, ENetworkFailure::Type failureType, const FString& errorString)
{
	DestroySession([] {
		UE_LOG(LogGameInstance, Display, TEXT("[UCustomGameInstanceBase] Detected Network Failure, destroyed session."));
		});

	// Load the main menu level
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), MainMenuLevel, true);
}

//-------------------------------------------------------------
//----------------  SESSION INVITES  --------------------------
//-------------------------------------------------------------
void UCustomGameInstanceBase::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 controllerId, FUniqueNetIdPtr userId, const FOnlineSessionSearchResult& inviteResult)
{
	if (IsInLobby)
	{
		TWeakObjectPtr weakThis = this;
		DestroySession([weakThis, bWasSuccessful, controllerId, userId, inviteResult] {
			if (weakThis.IsValid())
			{
				weakThis->OnSessionUserInviteAccepted(bWasSuccessful, controllerId, userId, inviteResult);
			}
			});
		return;
	}

	IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface.IsValid())
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
		OnJoinSessionCompleteDelegateHandle = sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
			FOnJoinSessionCompleteDelegate::CreateUObject(this, &UCustomGameInstanceBase::OnJoinSessionComplete));

		// Temp for 5.5, they aren't filling in the struct correctly
		if (!inviteResult.Session.SessionSettings.bIsDedicated)
		{
			FOnlineSessionSearchResult modResult = inviteResult;
			modResult.Session.SessionSettings.bUsesPresence = true;
			modResult.Session.SessionSettings.bUseLobbiesIfAvailable = true;
			JoinSessionAdvanced(modResult);
		}
		else
		{
			JoinSessionAdvanced(inviteResult);
		}
	}
	UE_LOG(LogGameInstance, Log, TEXT("Called Join Session for Steam Friends List UI InviteResults: %s, UserId: %s"), *inviteResult.GetSessionIdStr(), *userId->ToString());
}
void UCustomGameInstanceBase::OnPlayerLoginStatusChangedMaster(int32 playerNum, ELoginStatus::Type previousStatus, ELoginStatus::Type newStatus, const FUniqueNetId& newPlayerUniqueNetID)
{
	EBPLoginStatus origStatus = (EBPLoginStatus)previousStatus;
	EBPLoginStatus currentStatus = (EBPLoginStatus)newStatus;
	FBPUniqueNetId playerID;
	playerID.SetUniqueNetId(&newPlayerUniqueNetID);

	OnPlayerLoginStatusChanged(playerNum, origStatus, currentStatus, playerID);


	if (bCallIdentityInterfaceEventsOnPlayerControllers)
	{
		APlayerController* player = UGameplayStatics::GetPlayerController(GetWorld(), playerNum);

		if (player != NULL)
		{
			if (player->Implements<UAdvancedFriendsInterface>())
			{
				IAdvancedFriendsInterface::Execute_OnPlayerLoginStatusChanged(player, origStatus, currentStatus, playerID);
			}
		}
		else
		{
			UE_LOG(LogGameInstance, Warning, TEXT("[UCustomGameInstanceBase] Failed to get a controller with the specified index in OnPlayerLoginStatusChangedMaster!"));
		}
	}
}
void UCustomGameInstanceBase::OnPlayerLoginChangedMaster(int32 playerNum)
{
	OnPlayerLoginChanged(playerNum);

	if (bCallIdentityInterfaceEventsOnPlayerControllers)
	{
		APlayerController* player = UGameplayStatics::GetPlayerController(GetWorld(), playerNum);

		if (player != NULL)
		{
			//Run the Event specific to the actor, if the actor has the interface, otherwise ignore
			if (player->Implements<UAdvancedFriendsInterface>())
			{
				IAdvancedFriendsInterface::Execute_OnPlayerLoginChanged(player, playerNum);
			}
		}
		else
		{
			UE_LOG(LogGameInstance, Warning, TEXT("[UCustomGameInstanceBase] Failed to get a controller with the specified index in OnPlayerLoginChanged!"));
		}
	}
}
void UCustomGameInstanceBase::OnPlayerTalkingStateChangedMaster(TSharedRef<const FUniqueNetId> playerId, bool bIsTalking)
{
	FBPUniqueNetId playerTalking;
	playerTalking.SetUniqueNetId(playerId);
	OnPlayerTalkingStateChanged(playerTalking, bIsTalking);

	if (bCallVoiceInterfaceEventsOnPlayerControllers)
	{
		APlayerController* player = NULL;

		for (const ULocalPlayer* localPlayer : LocalPlayers)
		{
			player = UGameplayStatics::GetPlayerController(GetWorld(), localPlayer->GetControllerId());

			if (player != NULL)
			{
				//Run the Event specific to the actor, if the actor has the interface, otherwise ignore
				if (player->Implements<UAdvancedFriendsInterface>())
				{
					IAdvancedFriendsInterface::Execute_OnPlayerVoiceStateChanged(player, playerTalking, bIsTalking);
				}
			}
			else
			{
				UE_LOG(LogGameInstance, Warning, TEXT("[UCustomGameInstanceBase] Failed to get a controller with the specified index in OnVoiceStateChanged!"));
			}
		}
	}
}
void UCustomGameInstanceBase::OnSessionInviteReceivedMaster(const FUniqueNetId& personInvited, const FUniqueNetId& personInviting, const FString& appId, const FOnlineSessionSearchResult& sessionToJoin)
{
	if (sessionToJoin.IsValid())
	{
		FBlueprintSessionResult blueprintResult;
		blueprintResult.OnlineResult = sessionToJoin;

		FBPUniqueNetId invitedPlayer;
		invitedPlayer.SetUniqueNetId(&personInvited);

		FBPUniqueNetId invitingPlayer;
		invitingPlayer.SetUniqueNetId(&personInviting);


		TArray<APlayerController*> playerList;
		GEngine->GetAllLocalPlayerControllers(playerList);

		APlayerController* player = NULL;

		int32 localPlayer = 0;
		for (int i = 0; i < playerList.Num(); i++)
		{
			if (*playerList[i]->PlayerState->GetUniqueId().GetUniqueNetId() == personInvited)
			{
				localPlayer = i;
				player = playerList[i];
				break;
			}
		}

		// Temp for 5.5, they aren't filling in the struct correctly
		if (!blueprintResult.OnlineResult.Session.SessionSettings.bIsDedicated)
		{
			blueprintResult.OnlineResult.Session.SessionSettings.bUsesPresence = true;
			blueprintResult.OnlineResult.Session.SessionSettings.bUseLobbiesIfAvailable = true;
		}

		OnSessionInviteReceived(localPlayer, invitingPlayer, appId, blueprintResult);

		//IAdvancedFriendsInterface* TheInterface = NULL;

		if (player != NULL)
		{
			//Run the Event specific to the actor, if the actor has the interface, otherwise ignore
			if (player->GetClass()->ImplementsInterface(UAdvancedFriendsInterface::StaticClass()))
			{
				IAdvancedFriendsInterface::Execute_OnSessionInviteReceived(player, invitingPlayer, blueprintResult);
			}
		}
		else
		{
			UE_LOG(LogGameInstance, Warning, TEXT("[UCustomGameInstanceBase] Failed to get a controller with the specified index in OnSessionInviteReceived!"));
		}
	}
	else
	{
		UE_LOG(LogGameInstance, Warning, TEXT("[UCustomGameInstanceBase] Return a bad search result in OnSessionInviteReceived!"));
	}
}
void UCustomGameInstanceBase::OnSessionInviteAcceptedMaster(const bool bWasSuccessful, int32 localPlayer, TSharedPtr<const FUniqueNetId> personInvited, const FOnlineSessionSearchResult& sessionToJoin)
{
	if (bWasSuccessful)
	{
		if (sessionToJoin.IsValid())
		{

			FBlueprintSessionResult blueprintResult;
			blueprintResult.OnlineResult = sessionToJoin;

			FBPUniqueNetId invitedPlayer;
			invitedPlayer.SetUniqueNetId(personInvited);

			// Temp for 5.5, they aren't filling in the struct correctly
			if (!blueprintResult.OnlineResult.Session.SessionSettings.bIsDedicated)
			{
				blueprintResult.OnlineResult.Session.SessionSettings.bUsesPresence = true;
				blueprintResult.OnlineResult.Session.SessionSettings.bUseLobbiesIfAvailable = true;
			}

			OnSessionInviteAccepted(localPlayer, invitedPlayer, blueprintResult);

			APlayerController* player = UGameplayStatics::GetPlayerController(GetWorld(), localPlayer);

			//IAdvancedFriendsInterface* TheInterface = NULL;

			if (player != NULL)
			{
				//Run the Event specific to the actor, if the actor has the interface, otherwise ignore
				if (player->GetClass()->ImplementsInterface(UAdvancedFriendsInterface::StaticClass()))
				{
					IAdvancedFriendsInterface::Execute_OnSessionInviteAccepted(player, invitedPlayer, blueprintResult);
				}
			}
			else
			{
				UE_LOG(LogGameInstance, Warning, TEXT("[UCustomGameInstanceBase] Failed to get a controller with the specified index in OnSessionInviteAccepted!"));
			}
		}
		else
		{
			UE_LOG(LogGameInstance, Warning, TEXT("[UCustomGameInstanceBase] Return a bad search result in OnSessionInviteAccepted!"));
		}
	}
}

void UCustomGameInstanceBase::CreateAndEnterLobby(TSoftObjectPtr<UWorld> LobbyLevel, EOnlineSessionType OnlineSessionType)
{
	//in case player is in a session, destroy it first
	//can happen if host from previous session quit and this player was kicked back to main menu
	//then tries to start a new game
	DestroySession();

#if WITH_EDITOR
	// Act as if we are session host
	Debug_SetSessionHost(true);

	// Load Level
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), LobbyLevel, true, FString(L"?listen"));
#else
		StartNewParty(OnlineSessionType, [=, this](bool wasSuccessfull) {
		if (!wasSuccessfull)
		{
			UE_LOG(LogGameInstance, Warning, TEXT("Could not start new game."));
			return;
		}

		// Load Level
		UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), LobbyLevel, true, FString(L"?listen"));
		});
#endif
}

void UCustomGameInstanceBase::BeginLoadingScreen(const FString& MapName)
{
	UE_LOG(LogGameInstance, Warning, TEXT("[UCustomGameInstanceBase]Begin Loading screen: %s"), *MapName);

	FLoadingScreenAttributes LoadingScreen;

	LoadingScreen.MinimumLoadingScreenDisplayTime = 4.f;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;

	if (UUserWidget* LoadingWidget = CreateWidget<UUserWidget>(this, LoadingScreenWidgetClass))
	{
		LoadingWidget->AddToViewport();

		// Convert to Slate widget for movie player
		TSharedRef<SWidget> SlateWidget = LoadingWidget->TakeWidget();
		LoadingScreen.WidgetLoadingScreen = SlateWidget;
	}

	//LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			1,
			2.0f,
			FColor::Green,
			FString::Printf(TEXT("LOADING SCREEN")));
	}
}


void UCustomGameInstanceBase::EndLoadingScreen(UWorld* InLoadedWorld)
{
	UE_LOG(LogGameInstance, Warning, TEXT("[UCustomGameInstanceBase]End Loading screen: %s"), *InLoadedWorld->GetName());
}

//
//
//void UCustomGameInstanceBase::OnSeamlessTravelStart(UWorld* InWorld)
//{
//	//BeginLoadingScreen(TEXT("Seamless Travel Start"));
//
//	if (GEngine)
//	{
//		GEngine->AddOnScreenDebugMessage(
//			1,
//			2.0f,
//			FColor::Green,
//			FString::Printf(TEXT("LOADING SCREEN")));
//	}
//
//	TSharedRef<SWidget> LoadingOverlay = SNew(SBorder)
//		.HAlign(HAlign_Center)
//		.VAlign(VAlign_Center)
//		[
//			SNew(STextBlock).Text(FText::FromString(TEXT("Loading...")))
//		];
//
//	GEngine->GameViewport->AddViewportWidgetContent(LoadingOverlay, 1000);
//}
//
//void UCustomGameInstanceBase::OnSeamlessTravelComplete(UWorld* InWorld)
//{
//	EndLoadingScreen(GetWorld());
//}