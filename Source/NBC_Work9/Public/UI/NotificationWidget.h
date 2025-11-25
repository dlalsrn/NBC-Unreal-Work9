#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NotificationWidget.generated.h"

class UTextBlock;

UCLASS()
class NBC_WORK9_API UNotificationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateNotificationText();

	void UpdateRemainTimeText(int32 RemainingTime);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NotificationText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RemainTimeText;
};
