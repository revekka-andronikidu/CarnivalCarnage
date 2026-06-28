#include "OnlineHelpers.h"

// Online
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineExternalUIInterface.h"

DEFINE_LOG_CATEGORY(LogOnlineHelpers);

void UOnlineHelpers::OpenInviteFriendsOverlay()
{
	// Get the online subsystem (ensure it is Steam)
	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if (!onlineSubsystem || onlineSubsystem->GetSubsystemName() != FName("STEAM"))
	{
		UE_LOG(LogOnlineHelpers, Warning, TEXT("OnlineSubsystem is not Steam or unavailable!"));
		return;
	}

	// Get the external UI interface
	IOnlineExternalUIPtr externalUI = onlineSubsystem->GetExternalUIInterface();
	if (externalUI.IsValid())
	{
		// Show the Steam Overlay for Inviting Friends
		externalUI->ShowInviteUI(0);
		UE_LOG(LogOnlineHelpers, Log, TEXT("Invite Friends UI opened in Steam overlay."));
	}
	else
	{
		UE_LOG(LogOnlineHelpers, Error, TEXT("Failed to get External UI System for Steam!"));
	}
}
