#include "Game/NBGameModeBase.h"
#include "Game/NBGameStateBase.h"
#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"
#include "EngineUtils.h"

void ANBGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(NewPlayer);
	if (IsValid(NBPlayerController))
	{
		NBPlayerController->SetNotificationText(FText::FromString(TEXT("Connected to the game server.")));

		AllPlayerControllers.Add(NBPlayerController);

		ANBPlayerState* NBPS = NBPlayerController->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS))
		{
			NBPS->SetPlayerName(TEXT("Player") + FString::FromInt(AllPlayerControllers.Num()));
		}

		ANBGameStateBase* NBGameStateBase = GetGameState<ANBGameStateBase>();
		if (IsValid(NBGameStateBase))
		{
			NBGameStateBase->MulticastRPCBroadcastLoginMessage(NBPS->GetPlayerName());

			if (NBGameStateBase->GetCurrentTurnPlayerState() == nullptr)
			{
				NBGameStateBase->SetCurrentTurnPlayerState(AllPlayerControllers[0]->GetPlayerState<ANBPlayerState>());
				CurrentTurnIndex = 0;
			}
		}
	}
}

void ANBGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void ANBGameModeBase::StartGame()
{
	if (bIsGamePlaying)
	{
		return;
	}

	bIsGamePlaying = true;
	ResetGame();
	StartTurn();
}

FString ANBGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ANBGameModeBase::IsGuessNumber(const FString& InNumberString)
{
	bool bCanPlay = false;

	do
	{
		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0' || UniqueDigits.Contains(C))
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

FString ANBGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ANBGameModeBase::PrintChatMessage(ANBPlayerController* InChattingPlayerController, const FString& InChatMessage)
{
	ANBGameStateBase* GS = GetGameState<ANBGameStateBase>();
	if (!IsValid(GS) || !IsValid(InChattingPlayerController))
	{
		return;
	}

	ANBPlayerState* PS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
	if (!IsValid(PS))
	{
		return;
	}

	if (IsGuessNumber(InChatMessage))
	{
		if (!bIsGamePlaying)
		{
			InChattingPlayerController->ClientRPCPrintChatMessage(TEXT("SERVER: The game is not currently playing."));
			return;
		}

		if (GS->GetCurrentTurnPlayerState() != PS)
		{
			InChattingPlayerController->ClientRPCPrintChatMessage(TEXT("SERVER: It's not your turn."));
			return;
		}

		if (PS->GetCurrentGuessCount() >= PS->GetMaxGuessCount())
		{
			InChattingPlayerController->ClientRPCPrintChatMessage(TEXT("SERVER: You have reached the maximum number of guesses."));
			return;
		}

		GetWorldTimerManager().ClearTimer(TurnTimerHandle);
		IncreaseGuessCount(InChattingPlayerController);

		FString PlayerChatString = PS->GetPlayerInfo() + TEXT(": ") + InChatMessage;
		GS->MulticastRPCBroadcastChatMessage(PlayerChatString);

		FString JudgeResultString = JudgeResult(SecretNumberString, InChatMessage);
		FString ServerJudgeMessage = FString::Printf(TEXT("SERVER: %s -> %s"), *InChatMessage, *JudgeResultString);
		GS->MulticastRPCBroadcastJudgeResult(ServerJudgeMessage);

		NextTurn();

		int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
		if (JudgeGame(InChattingPlayerController, StrikeCount))
		{
			bIsGamePlaying = false;
			return;
		}

		BroadcastCurrentTurnNotification();
	}
	else
	{
		FString PlayerChatString = PS->GetPlayerInfo() + TEXT(": ") + InChatMessage;
		GS->MulticastRPCBroadcastChatMessage(PlayerChatString);
	}
}

void ANBGameModeBase::IncreaseGuessCount(ANBPlayerController* InChattingPlayerController)
{
	ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
	if (IsValid(NBPS))
	{
		NBPS->IncreaseCurrentGuessCount();
	}
}

void ANBGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();
	UE_LOG(LogTemp, Error, TEXT("Secret Number: %s"), *SecretNumberString);

	ANBGameStateBase* NBGameStateBase = GetGameState<ANBGameStateBase>();
	if (IsValid(NBGameStateBase))
	{
		NBGameStateBase->SetCurrentTurnPlayerState(AllPlayerControllers[0]->GetPlayerState<ANBPlayerState>());
		CurrentTurnIndex = 0;

		ANBPlayerState* CurrentPS = NBGameStateBase->GetCurrentTurnPlayerState();
		if (IsValid(CurrentPS))
		{
			FString CombineMessagString = TEXT("Current Turn: ") + CurrentPS->GetPlayerName();

			for (const auto& NBPlayerController : AllPlayerControllers)
			{
				NBPlayerController->SetNotificationText(FText::FromString(CombineMessagString));

				ANBPlayerState* NBPS = NBPlayerController->GetPlayerState<ANBPlayerState>();
				if (IsValid(NBPS))
				{
					NBPS->SetCurrentGuessCount(0);
				}
			}
		}
	}
}

bool ANBGameModeBase::JudgeGame(ANBPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (InStrikeCount == 3)
	{
		ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
		for (const auto& NBPlayerController : AllPlayerControllers)
		{
			if (IsValid(NBPS))
			{
				FString CombinedMessageString = NBPS->GetPlayerName() + TEXT(" has won the game.");
				NBPlayerController->SetNotificationText(FText::FromString(CombinedMessageString));
			}
		}

		return true;
	}
	else
	{
		for (const auto& NBPlayerController : AllPlayerControllers)
		{
			ANBPlayerState* NBPS = NBPlayerController->GetPlayerState<ANBPlayerState>();
			if (IsValid(NBPS))
			{
				if (NBPS->GetCurrentGuessCount() < NBPS->GetMaxGuessCount())
				{
					return false;
				}
			}
		}

		for (const auto& NBPlayerController : AllPlayerControllers)
		{
			NBPlayerController->SetNotificationText(FText::FromString(TEXT("Draw...")));
		}

		return true;
	}
}

TArray<ANBPlayerController*> ANBGameModeBase::GetAllPlayerControllers() const
{
	return AllPlayerControllers;
}

bool ANBGameModeBase::GetIsGamePlaying() const
{
	return bIsGamePlaying;
}

void ANBGameModeBase::StartTurn()
{
	ANBGameStateBase* GS = GetGameState<ANBGameStateBase>();
	GS->SetRemainingTurnTime(TurnDuration);

	GetWorldTimerManager().ClearTimer(TurnTimerHandle);
	GetWorldTimerManager().SetTimer(
		TurnTimerHandle,
		this,
		&ANBGameModeBase::DecreaseTurnTime,
		1.0f,
		true
	);
}

void ANBGameModeBase::NextTurn()
{
	CurrentTurnIndex = (CurrentTurnIndex + 1) % AllPlayerControllers.Num();
	ANBGameStateBase* NBGS = GetGameState<ANBGameStateBase>();
	if (IsValid(NBGS))
	{
		ANBPlayerState* NextPS = AllPlayerControllers[CurrentTurnIndex]->GetPlayerState<ANBPlayerState>();
		NBGS->SetCurrentTurnPlayerState(NextPS);
	}
}

void ANBGameModeBase::BroadcastCurrentTurnNotification()
{
	ANBGameStateBase* GS = GetGameState<ANBGameStateBase>();
	if (IsValid(GS))
	{
		GS->MulticastRPCBroadcastCurrentTurnNotification(GS->GetCurrentTurnPlayerState());
	}

	StartTurn();
}

void ANBGameModeBase::DecreaseTurnTime()
{
	ANBGameStateBase* NBGS = GetGameState<ANBGameStateBase>();
	if (IsValid(NBGS))
	{
		int32 RemainingTurnTime = NBGS->GetRemainingTurnTime();
		RemainingTurnTime = FMath::Max(RemainingTurnTime - 1, 0);
		NBGS->SetRemainingTurnTime(RemainingTurnTime);

		if (RemainingTurnTime <= 0)
		{
			GetWorldTimerManager().ClearTimer(TurnTimerHandle);

			FString TimeLimitMessage = FString::Printf(TEXT("SERVER: %s Time limit exceeded."), *NBGS->GetCurrentTurnPlayerState()->GetPlayerName());
			NBGS->MulticastRPCBroadcastChatMessage(TimeLimitMessage);

			ANBPlayerState* PS = NBGS->GetCurrentTurnPlayerState();
			if (IsValid(PS))
			{
				PS->IncreaseCurrentGuessCount();

				NextTurn();

				ANBPlayerController* CurrentTurnPC = Cast<ANBPlayerController>(PS->GetPlayerController());
				if (JudgeGame(CurrentTurnPC, 0))
				{
					bIsGamePlaying = false;
					return;
				}

				BroadcastCurrentTurnNotification();
			}

		}
	}
}
