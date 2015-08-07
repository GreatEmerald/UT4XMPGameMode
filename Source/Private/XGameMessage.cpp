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

#include "XPlayerController.h"

#include "XDeployPoint.h"
#include "XEnergySource.h"
#include "XPlayerState.h"
#include "XArtifactNode.h"

#include "XGameMessage.h"

UXGameMessage::UXGameMessage(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MessageArea = FName(TEXT("GameMessages"));
	bIsUnique = true;
	Lifetime = 2.0f;
	bIsStatusAnnouncement = true;
	bOptionalSpoken = true; // @TODO FIXMESTEVE - depends on the message - some should just delay!

	// flag related
	MessageText.Add(NSLOCTEXT("UXGameMessage", "AllFlags", "All flags are in play."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "BlueAllFlags", "Blue team has captured all flags."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "RedAllFlags", "Red team has captured all flags."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "EnemyCapFlag", "The enemy has captured a flag."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "EnemyFlagReturned", "Flag returned to the enemy base."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "EnemyTakenFlags", "The enemy has taken all your flags."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "EnemyTakenFlag", "A flag has been taken fron your base."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "TeamCapturedFlag", "Your team has captured a flag."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "FlagsCritical", "Flags critical."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "TeamFlagReturned", "Flag returned to your base."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "TeamControlsFlags", "Your team controls all flags."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "StoleEnemyFlag", "Enemy flag stolen."));
	// spawn point related
	MessageText.Add(NSLOCTEXT("UXGameMessage", "EnemyControlsSpawnPoints", "The enemy controls all spawn points."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "EnemyGainedSpawnPoint", "The enemy team has gained a spawn point."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "TeamLostSpawnPoint", "Your team has lost a spawn point."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "TeamControllsAllSpawnPoints", "Your team controls all spawn points."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "TeamStolenSpawnPoint", "Your team has stolen a spawn point."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "TeamGainedSpawnPoint", "Your team has gained a spawn point."));
	// end of round related
	MessageText.Add(NSLOCTEXT("UXGameMessage", "BlueWins", "Blue team wins."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "BlueWonRound", "Blue team has won the round."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "BlueWonMatch", "Blue team has won the match."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "RedEnergyDepleted", "Red team energy depleted."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "TimeLimitMet", "Time limit met."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "RedWins", "Red team wins."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "RedWonRound", "Red team has won the round."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "RedWonMatch", "Red team has won the match."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "BlueEnergyDepleted", "Blue team energy depleted."));
	// energy source related
	MessageText.Add(NSLOCTEXT("UXGameMessage", "EnemyGainedEnergy", "The enemy has gained an energy source."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "TeamLostEnergy", "Your team has lost an energy source."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "TeamStoleEnergy", "Your team has stolen an energy source."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "TeamGainedEnergy", "Your team has gained an energy source."));
	// rest
	MessageText.Add(NSLOCTEXT("UXGameMessage", "EnemyDominating", "The enemy team is dominating."));
	MessageText.Add(NSLOCTEXT("UXGameMessage", "TeamDominating", "Your team is dominating."));

	MessageAnnouncements.Add(TEXT("vo_AnyTeam_Flag_Carry_All_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_End_BlueVictory_Flag_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_End_RedVictory_Flag_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_EnemyTeam_Flag_Capture_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_EnemyTeam_Flag_Return_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_EnemyTeam_Flag_Stolen_All_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_EnemyTeam_Flag_Stolen_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_PlayerTeam_Flag_Capture_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_PlayerTeam_Flag_Critical_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_PlayerTeam_Flag_Return_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_PlayerTeam_Flag_Stolen_All_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_PlayerTeam_Flag_Stolen_tts_Cue"));

	MessageAnnouncements.Add(TEXT("vo_EnemyTeam_Dominate_Spawn_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_EnemyTeam_SpawnPoint_Gain_FromIdle_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_EnemyTeam_SpawnPoint_Gain_FromPlayer_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_PlayerTeam_Dominate_Spawn_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_PlayerTeam_SpawnPoint_Gain_FromEnemy_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_PlayerTeam_SpawnPoint_Gain_FromIdle_tts_Cue"));

	MessageAnnouncements.Add(TEXT("vo_End_BlueVictory_01_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_End_BlueVictory_02_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_End_BlueVictory_02_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_End_BlueVictory_Energy_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_End_General_Time_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_End_RedVictory_01_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_End_RedVictory_02_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_End_RedVictory_03_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_End_RedVictory_Energy_tts_Cue"));

	MessageAnnouncements.Add(TEXT("vo_EnemyTeam_EnergySource_Gain_FromIdle_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_EnemyTeam_EnergySource_Gain_FromPlayer_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_PlayerTeam_EnergySource_Gain_FromEnemy_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_PlayerTeam_EnergySource_Gain_FromIdle_tts_Cue"));

	MessageAnnouncements.Add(TEXT("vo_EnemyTeam_Dominate_Everything_tts_Cue"));
	MessageAnnouncements.Add(TEXT("vo_PlayerTeam_Dominate_Everything_tts_Cue"));



}

FLinearColor UXGameMessage::GetMessageColor(int32 MessageIndex) const
{
	return FLinearColor::Yellow;

	if (MessageIndex <= 11)return FLinearColor::Yellow;
	else if (12 <= MessageIndex && MessageIndex <= 17) return FLinearColor::Green;
	else if (17 < MessageIndex && MessageIndex <= 26) return FLinearColor::Blue;
	else return FLinearColor::Yellow;
}

void UXGameMessage::PrecacheAnnouncements_Implementation(UUTAnnouncer* Announcer) const
{
	for (int32 i = 0; i <= 32; i++)
	{
		Announcer->PrecacheAnnouncement(MessageAnnouncements[i]);
	}
}

bool UXGameMessage::InterruptAnnouncement_Implementation(int32 Switch, const UObject* OptionalObject, TSubclassOf<UUTLocalMessage> OtherMessageClass, int32 OtherSwitch, const UObject* OtherOptionalObject) const
{
	if (Cast<UUTLocalMessage>(OtherMessageClass->GetDefaultObject())->bOptionalSpoken)
	{
		return true;
	}
	if (GetClass() == OtherMessageClass)
	{
		if ((OtherSwitch == 2) || (OtherSwitch == 8) || (OtherSwitch == 9))
		{
			// never interrupt scoring announcements
			return false;
		}
		if (OptionalObject == OtherOptionalObject)
		{
			// interrupt announcement about same object
			return true;
		}
	}
	return false;
}

FName UXGameMessage::GetAnnouncementName_Implementation(int32 Switch, const UObject* OptionalObject) const
{
	return MessageAnnouncements[Switch];
}

FText UXGameMessage::GetText(int32 Switch, bool bTargetsPlayerState1, class APlayerState* RelatedPlayerState_1, class APlayerState* RelatedPlayerState_2, class UObject* OptionalObject) const
{
	return (Switch >= 0 && Switch < MessageText.Num() ? MessageText[Switch] : FText::GetEmpty());
}

void UXGameMessage::ClientReceive(const FClientReceiveData& ClientData) const
{
	AXPlayerController* PC = Cast<AXPlayerController>(ClientData.LocalPC);
	AXPlayerState* PS = Cast<AXPlayerState>(PC->PlayerState);
	int32 NewIndex = ClientData.MessageIndex;

	if (PC && (PC->PlayerState != NULL) && !PC->PlayerState->bOnlySpectator)
	{
		if (PC->XGameAnnouncer)
		{
			if (ClientData.OptionalObject && ClientData.OptionalObject->IsA(AXEnergySource::StaticClass()))
			{
				const AXEnergySource* Source = Cast<AXEnergySource>(ClientData.OptionalObject);
				bool bMyTeam = (Source->TeamNum == PS->GetTeamNum());

				switch (ClientData.MessageIndex)
				{
				case 27: bMyTeam ? NewIndex = 30 : NewIndex = 27; break;
				}

			}

			else if (ClientData.OptionalObject && ClientData.OptionalObject->IsA(AXDeployPoint::StaticClass()))
			{
				const AXDeployPoint* Point = Cast<AXDeployPoint>(ClientData.OptionalObject);
			}

			else if (ClientData.OptionalObject && ClientData.OptionalObject->IsA(AXArtifactNode::StaticClass()))
			{
				const AXArtifactNode* Node = Cast<AXArtifactNode>(ClientData.OptionalObject);
			}

			PC->XGameAnnouncer->PlayAnnouncement(GetClass(), NewIndex, PC->PlayerState, nullptr, ClientData.OptionalObject);
		}
	}

	Super::ClientReceive(ClientData);
}
