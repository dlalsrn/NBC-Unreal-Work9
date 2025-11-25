#include "Game/NBGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"
#include "Net/UnrealNetwork.h"

void ANBGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (!HasAuthority())
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC))
		{
			ANBPlayerController* NBPC = Cast<ANBPlayerController>(PC);
			if (IsValid(NBPC))
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				NBPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}

void ANBGameStateBase::MulticastRPCBroadcastCurrentTurnNotification_Implementation(ANBPlayerState* InTurnPlayerState)
{
	if (!HasAuthority())
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC))
		{
			ANBPlayerController* NBPC = Cast<ANBPlayerController>(PC);
			if (IsValid(NBPC))
			{
				if (IsValid(InTurnPlayerState))
				{
					FString NotificationString = TEXT("Current Turn: ") + InTurnPlayerState->GetPlayerName();
					NBPC->SetNotificationText(FText::FromString(NotificationString));
				}
			}
		}
	}
}

void ANBGameStateBase::MulticastRPCBroadcastChatMessage_Implementation(const FString& InMessage)
{
	if (!HasAuthority())
	{
		ANBPlayerController* PC = Cast<ANBPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (IsValid(PC))
		{
			PC->ClientRPCPrintChatMessage(InMessage);
		}
	}
}

void ANBGameStateBase::MulticastRPCBroadcastJudgeResult_Implementation(const FString& InJudgeResult)
{
	if (!HasAuthority())
	{
		ANBPlayerController* PC = Cast<ANBPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (IsValid(PC))
		{
			PC->ClientRPCPrintChatMessage(InJudgeResult);
		}
	}
}

void ANBGameStateBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentTurnPlayerState);
	DOREPLIFETIME(ThisClass, RemainingTurnTime);
}

void ANBGameStateBase::SetCurrentTurnPlayerState(ANBPlayerState* InPlayerState)
{
	CurrentTurnPlayerState = InPlayerState;
}

ANBPlayerState* ANBGameStateBase::GetCurrentTurnPlayerState() const
{
	return CurrentTurnPlayerState;
}

int32 ANBGameStateBase::GetRemainingTurnTime() const
{
	return RemainingTurnTime;
}

void ANBGameStateBase::SetRemainingTurnTime(int32 InRemainingTurnTime)
{
	RemainingTurnTime = InRemainingTurnTime;
}

void ANBGameStateBase::OnRep_CurrentTurnPlayerState()
{
	ANBPlayerController* PC = Cast<ANBPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IsValid(PC))
	{
		if (IsValid(CurrentTurnPlayerState))
		{
			FString NotificationString = TEXT("Current Turn: ") + CurrentTurnPlayerState->GetPlayerName();
			PC->SetNotificationText(FText::FromString(NotificationString));
		}
	}
}

void ANBGameStateBase::OnRep_RemainingTurnTime()
{
	ANBPlayerController* PC = Cast<ANBPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IsValid(PC))
	{
		PC->SetTimeNotificationText(RemainingTurnTime);
	}
}
