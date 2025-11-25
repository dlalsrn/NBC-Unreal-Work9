#include "Player/NBPlayerController.h"
#include "Game/NBGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "NBC_Work9/NBC_Work9.h"
#include "UI/ChatInputWidget.h"
#include "UI/NotificationWidget.h"
#include "Net/UnrealNetwork.h"

ANBPlayerController::ANBPlayerController()
{
	bReplicates = true;
}

void ANBPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void ANBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass))
	{
		ChatInputWidgetInstance = CreateWidget<UChatInputWidget>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance))
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass))
	{
		NotificationTextWidgetInstance = CreateWidget<UNotificationWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance))
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void ANBPlayerController::ClientRPCPrintChatMessage_Implementation(const FString& ChatMessage)
{
	PrintChatMessageString(ChatMessage);
}

void ANBPlayerController::ServerRPCPrintChatMessage_Implementation(const FString& ChatMessage)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM))
	{
		ANBGameModeBase* NBGM = Cast<ANBGameModeBase>(GM);
		if (IsValid(NBGM))
		{
			NBGM->PrintChatMessage(this, ChatMessage);
		}
	}
}

bool ANBPlayerController::ServerRPCPrintChatMessage_Validate(const FString& ChatMessage)
{
	return true;
}

void ANBPlayerController::ServerRPCRequestStartGame_Implementation()
{
	ANBGameModeBase* GM = GetWorld()->GetAuthGameMode<ANBGameModeBase>();
	if (IsValid(GM))
	{
		GM->StartGame();
	}
}

void ANBPlayerController::RequestStartGame()
{
	ServerRPCRequestStartGame();
}

void ANBPlayerController::SetChatMessage(const FString& ChatMessage)
{
	if (IsLocalController())
	{
		ServerRPCPrintChatMessage(ChatMessage);
	}
}

void ANBPlayerController::PrintChatMessageString(const FString& ChatMessage)
{
	NBFunctionLibrary::MyPrintString(this, ChatMessage, 10.0f);
}

FText ANBPlayerController::GetNotificationText() const
{
	return NotificationText;
}

void ANBPlayerController::SetNotificationText(const FText& InNotificationText)
{
	NotificationText = InNotificationText;
}

void ANBPlayerController::SetTimeNotificationText(int32 RemainingTime)
{
	if (IsValid(NotificationTextWidgetInstance))
	{
		NotificationTextWidgetInstance->UpdateRemainTimeText(RemainingTime);
	}
}
