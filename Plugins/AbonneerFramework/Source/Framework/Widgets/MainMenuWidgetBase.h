#pragma once

#include "CoreMinimal.h"
#include "Online/AdvancedSessions/BlueprintDataDefinitions.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidgetBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMainMenu, Log, All);

/**
 * Main menu, that's it.
 */
class UButton;

UCLASS(Blueprintable)
class FRAMEWORK_API UMainMenuWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable)
	void StartGame();
	UFUNCTION(BlueprintCallable)
	void QuitGame();

	// Session Type
	UFUNCTION(BlueprintCallable)
	void SetOnlineSessionType(EOnlineSessionType type) { OnlineSessionType = type; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EOnlineSessionType GetOnlineSessionType() const { return OnlineSessionType; }

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_CreateLobby;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_Quit;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_Settings;

	UFUNCTION()
	void OnButton_CreateLobbyClicked();

	UFUNCTION()
	void OnButton_QuitClicked();

	UFUNCTION()
	void OnButton_SettingsClicked();

protected:

	// Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|MainMenu")
	TSoftObjectPtr<UWorld> DefaultLevel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|MainMenu")
	TSubclassOf<AGameModeBase> DefaultGameMode;

	// Session Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|MainMenu")
	EOnlineSessionType OnlineSessionType = EOnlineSessionType::Public;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|MainMenu")
	UUserWidget* SettingsWidgetRef;


private:

};
