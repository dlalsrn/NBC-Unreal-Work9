#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NBPlayerState.generated.h"

UCLASS()
class NBC_WORK9_API ANBPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ANBPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	FString GetPlayerName() const;
	void SetPlayerName(const FString& NewPlayerName);
	FString GetPlayerInfo();

	int32 GetCurrentGuessCount() const;
	int32 GetMaxGuessCount() const;
	void SetCurrentGuessCount(int32 NewGuessCount);
	void IncreaseCurrentGuessCount();

protected:
	UPROPERTY(Replicated)
	FString PlayerName;

	UPROPERTY(Replicated)
	int32 CurrentGuessCount;

	UPROPERTY(Replicated)
	int32 MaxGuessCount;
};
