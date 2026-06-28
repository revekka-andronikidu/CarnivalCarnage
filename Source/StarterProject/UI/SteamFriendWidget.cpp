// Copyright Abonneer, Inc. All Rights Reserved.

#include "SteamFriendWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Online/AdvancedSteamSessions/AdvancedSteamFriendsLibrary.h"
#include "Online/AdvancedSessions/AdvancedFriendsLibrary.h"
#include "GameFramework/PlayerController.h"

void USteamFriendWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button_Invite)
    {
        Button_Invite->OnClicked.AddDynamic(this, &USteamFriendWidget::OnButton_InviteClicked);
    }
}

void USteamFriendWidget::OnButton_InviteClicked()
{
   /* APlayerController* PC = GetOwningPlayer();
    EBlueprintResultSwitch Result;
    UAdvancedFriendsLibrary::SendSessionInviteToFriend(PC, FriendData.UniqueNetId, Result);

    switch (Result)
    {
    case EBlueprintResultSwitch::OnSuccess:
        UE_LOG(LogTemp, Log, TEXT("Session invite sent successfully!"));
        break;

    case EBlueprintResultSwitch::OnFailure:
        UE_LOG(LogTemp, Warning, TEXT("Failed to send session invite."));
        break;

    default:
        break;
    }*/
}

void USteamFriendWidget::UpdateTab(FBPFriendInfo steamFriendData)
{

	FriendData = steamFriendData;

    if (T_PlayerName)
    {
        T_PlayerName->SetText(FText::FromString(FriendData.DisplayName));
    }

    //Grey out if friend is offline
    //BUG: does not show up in development build but should be working in shipping build
    if (FriendData.OnlineState == EBPOnlinePresenceState::Offline)
    {
        this->SetRenderOpacity(0.8f);
    }
    else
    {
        this->SetRenderOpacity(1.f);
    }

    //For now calling this from blue print due to liner err
    //EBlueprintAsyncResultSwitch Result;

   /* UTexture2D* AvatarTex = UAdvancedSteamFriendsLibrary::GetSteamFriendAvatar(FriendData.UniqueNetId, Result, SteamAvatarSize::SteamAvatar_Medium);

    if (Result == EBlueprintAsyncResultSwitch::OnSuccess && AvatarTex && Image_Avatar)
    {
        Image_Avatar->SetBrushFromTexture(AvatarTex);
    }*/
}