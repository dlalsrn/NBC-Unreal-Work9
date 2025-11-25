#include "UI/ChatInputWidget.h"
#include "Components/EditableTextBox.h"
#include "Player/NBPlayerController.h"

void UChatInputWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted))
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UChatInputWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted))
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UChatInputWidget::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			if (ANBPlayerController* NBPC = Cast<ANBPlayerController>(PC))
			{
				NBPC->SetChatMessage(Text.ToString());
				EditableTextBox_ChatInput->SetText(FText());
			}
		}
	}
}