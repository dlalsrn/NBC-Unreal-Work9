#include "Player/NBPlayerState.h"
#include "Net/UnrealNetwork.h"

ANBPlayerState::ANBPlayerState() :
	PlayerName(TEXT("")),
	CurrentGuessCount(0),
	MaxGuessCount(3)
{
	bReplicates = true;
}

void ANBPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerName);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
}

FString ANBPlayerState::GetPlayerName() const
{
	return PlayerName;
}

void ANBPlayerState::SetPlayerName(const FString& NewPlayerName)
{
	PlayerName = NewPlayerName;
}

FString ANBPlayerState::GetPlayerInfo()
{
	FString PlayerInfoString = PlayerName + TEXT("(") + FString::FromInt(CurrentGuessCount) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");
	return PlayerInfoString;
}

int32 ANBPlayerState::GetCurrentGuessCount() const
{
	return CurrentGuessCount;
}

int32 ANBPlayerState::GetMaxGuessCount() const
{
	return MaxGuessCount;
}

void ANBPlayerState::SetCurrentGuessCount(int32 NewGuessCount)
{
	CurrentGuessCount = NewGuessCount;
}

void ANBPlayerState::IncreaseCurrentGuessCount()
{
	CurrentGuessCount++;
}
