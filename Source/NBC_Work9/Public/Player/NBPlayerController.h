#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NBPlayerController.generated.h"

class UChatInputWidget;
class UNotificationWidget;

UCLASS()
class NBC_WORK9_API ANBPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ANBPlayerController();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessage(const FString& ChatMessage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCPrintChatMessage(const FString& ChatMessage);

	UFUNCTION(Server, Reliable)
	void ServerRPCRequestStartGame();

	UFUNCTION(BlueprintCallable)
	void RequestStartGame();

	void SetChatMessage(const FString& ChatMessage);

	void PrintChatMessageString(const FString& ChatMessage);

	FText GetNotificationText() const;
	void SetNotificationText(const FText& InNotificationText);

	void SetTimeNotificationText(int32 RemainingTime);

	
protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(Replicated)
	FText NotificationText;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UChatInputWidget> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UChatInputWidget> ChatInputWidgetInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNotificationWidget> NotificationTextWidgetClass;

	UPROPERTY()
	TObjectPtr<UNotificationWidget> NotificationTextWidgetInstance;
};
