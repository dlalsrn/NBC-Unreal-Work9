#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NBGameStateBase.generated.h"

class ANBPlayerState;

UCLASS()
class NBC_WORK9_API ANBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXXXXX")));

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastCurrentTurnNotification(ANBPlayerState* InTurnPlayerState);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastChatMessage(const FString& InMessage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastJudgeResult(const FString& InJudgeResult);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void SetCurrentTurnPlayerState(ANBPlayerState* InPlayerState);
	ANBPlayerState* GetCurrentTurnPlayerState() const;

	int32 GetRemainingTurnTime() const;
	void SetRemainingTurnTime(int32 InRemainingTurnTime);

	UFUNCTION()
	void OnRep_CurrentTurnPlayerState();

	UFUNCTION()
	void OnRep_RemainingTurnTime();

protected:
	UPROPERTY(Replicated)
	TObjectPtr<ANBPlayerState> CurrentTurnPlayerState = nullptr;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_RemainingTurnTime)
	int32 RemainingTurnTime;
};
