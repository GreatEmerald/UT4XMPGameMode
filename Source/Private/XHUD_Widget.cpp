/* Copyright (C) 2015 saiboat
 * Copyright (C) 2015 Dainius "GreatEmerald" Masiliunas
 *
 * This file is part of UT4XMP.
 *
 * UT4XMP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * UT4XMP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with UT4XMP. If not, see <http://www.gnu.org/licenses/>.
 *
 * Additional permission under GNU GPL version 3 section 7:
 *
 * If you modify this Program, or any covered work, by linking or combining it
 * with Unreal Engine 4 (or a modified version of that program), containing
 * parts covered by the terms of the Unreal Engine End User License Agreement,
 * the licensors of this Program grant you additional permission to convey the
 * resulting work.
 */
#include "XGameMode.h"
#include "XHUD_Widget.h"
#include "XPlayerController.h"
#include "XPlayerState.h"

UXHUD_Widget::UXHUD_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UXHUD_Widget::ClickedSpawn()
{
	ClassMenuCode->SetVisibility(ESlateVisibility::Hidden);
	FInputModeGameOnly InputMode;
	OwningXPlayer->SetInputMode(InputMode);
	OwningXPlayer->PlayerState->bOnlySpectator = false;
	OwningXPlayer->bShowMouseCursor = false;
	OwningXPlayer->bShowClassMenu = false;

	AXPlayerState* PS = Cast<AXPlayerState>(OwningXPlayer->PlayerState);
	if (SelectedTeam.Equals("Red"))
	{

		if (PS != NULL)
		{
			if (PS->Team != NULL && PS->Team->TeamIndex == 0) return;
			PS->ServerRequestChangeTeam(0);
			OwningXPlayer->ChangeState(FName(TEXT("NAME_WaitingToSpawn")));
			OwningXPlayer->ServerViewNextRespawnPoint();
			// GetWorld()->GetAuthGameMode<AXGameMode>()->RestartPlayer(OwningXPlayer);
		}
	}
	else
	{
		if (PS != NULL)
		{
			if (PS->Team != NULL && PS->Team->TeamIndex == 1) return;
			PS->ServerRequestChangeTeam(1);
			OwningXPlayer->ChangeState(FName(TEXT("NAME_WaitingToSpawn")));
			OwningXPlayer->ServerViewNextRespawnPoint();
			// GetWorld()->GetAuthGameMode<AXGameMode>()->RestartPlayer(OwningXPlayer);
		}
	}

}

void UXHUD_Widget::ClickedSpectate()
{
	AXPlayerState* PS = Cast<AXPlayerState>(OwningXPlayer->PlayerState);

	if (OwningXPlayer->IsInState(NAME_Spectating))
	{
		ClassMenuCode->SetVisibility(ESlateVisibility::Hidden);
		// GetWorld()->GetAuthGameMode<AXGameMode>()->StartNewPlayer(OwningXPlayer); wat?

	}
	else
	{
		ClassMenuCode->SetVisibility(ESlateVisibility::Hidden);
		OwningXPlayer->GoToSpectateMode();
		if (PS->Team != NULL)
		{
			PS->Team->RemoveFromTeam(OwningXPlayer);
			// GetWorld()->GetAuthGameMode<AXGameMode>()->RestartPlayer(OwningXPlayer);
		}
	}
	FInputModeGameOnly InputMode;
	OwningXPlayer->SetInputMode(InputMode);
	OwningXPlayer->bShowClassMenu = false;
	OwningXPlayer->bShowMouseCursor = false;
}
