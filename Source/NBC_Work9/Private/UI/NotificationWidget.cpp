#include "UI/NotificationWidget.h"
#include "Components/TextBlock.h"
#include "Player/NBPlayerController.h"

void UNotificationWidget::UpdateNotificationText()
{
	ANBPlayerController* NBPC = Cast<ANBPlayerController>(GetOwningPlayer());
	if (IsValid(NBPC))
	{
		NotificationText->SetText(NBPC->GetNotificationText());
	}
}

void UNotificationWidget::UpdateRemainTimeText(int32 RemainingTime)
{
	RemainTimeText->SetText(FText::FromString(FString::Printf(TEXT("%d"), RemainingTime)));
}
