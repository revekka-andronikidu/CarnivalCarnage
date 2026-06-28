// Copyright Abonneer, Inc. All Rights Reserved.


#include "LeaderboardWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "StarterProject/PlayerController/MainPlayerState.h"
#include "StarterProject/States/GameplayGameState.h"
#include "StarterProject/States/EndScreenGameState.h"

void ULeaderboardWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (auto GS = GetWorld()->GetGameState<AGameplayGameState>())
    {
        GS->OnLeaderboardChanged.AddDynamic(this, &ULeaderboardWidget::RefreshLeaderboard);
    }
}

void ULeaderboardWidget::RefreshLeaderboard()
{
    Entries.Empty();


    AGameStateBase* GS = GetWorld()->GetGameState();
    if (!GS)
        return;

    for (APlayerState* PS : GS->PlayerArray)
    {
        if (AMainPlayerState* MPS = Cast<AMainPlayerState>(PS))
        {
            auto Entry = MPS->GetLeaderboardEntry();
            Entries.Add(Entry);

            UE_LOG(LogTemp, Log, TEXT("Adding entry: PlayerName=%s, Score=%d"),
                *Entry.PlayerName, Entry.PlayerScore);
        }
    }

    Entries.Sort([](const FLeaderboardEntry& A, const FLeaderboardEntry& B)
             {
                 return A.PlayerScore > B.PlayerScore;
             });

    UpdateLeaderboard();
}