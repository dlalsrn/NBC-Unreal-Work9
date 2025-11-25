#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NBGameModeBase.generated.h"

class ANBPlayerController;

UCLASS()
class NBC_WORK9_API ANBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void OnPostLogin(AController* NewPlayer) override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void StartGame();

	UFUNCTION(BlueprintPure)
	bool GetIsGamePlaying() const;

	FString GenerateSecretNumber();

	bool IsGuessNumber(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	void PrintChatMessage(ANBPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(ANBPlayerController* InChattingPlayerController);

	void ResetGame();

	bool JudgeGame(ANBPlayerController* InChattingPlayerController, int InStrikeCount);

	TArray<ANBPlayerController*> GetAllPlayerControllers() const;


private:
	void StartTurn();
	void NextTurn();
	void BroadcastCurrentTurnNotification();

	void DecreaseTurnTime();

protected:
	FString SecretNumberString;

	TArray<TObjectPtr<ANBPlayerController>> AllPlayerControllers;

	int32 CurrentTurnIndex;

	bool bIsGamePlaying = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "Game Rule")
	int32 TurnDuration;

private:
	FTimerHandle NextTurnTimerHandle;

	FTimerHandle TurnTimerHandle;
};
