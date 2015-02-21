/* Copyright (C) 2015 Dainius "GreatEmerald" Masiliunas
 *
 * This file is part of UT4XMP Game Mode.
 *
 * UT4XMP Game Mode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * UT4XMP Game Mode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with UT4XMP Game Mode. If not, see <http://www.gnu.org/licenses/>.
 *
 * Additional permission under GNU GPL version 3 section 7:
 *
 * If you modify this Program, or any covered work, by linking or combining it
 * with Unreal Engine 4 (or a modified version of that program), containing
 * parts covered by the terms of the Unreal Engine End User License Agreement,
 * the licensors of this Program grant you additional permission to convey the
 * resulting work.
 */

#include "UnrealTournament.h"
#include "XMPArtifactNode.h"
#include "XMPGameMode.h"

// GEm: class XMPGameMode extends UTCTFGameMode;
#define SELF AXMPGameMode

// GEm: DefaultProperties
SELF::SELF(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GoalScore = 4;
}

void SELF::BeginPlay()
{
	int i, n;
	AXMPArtifactNode* ArtifactNode;

	//UE_LOG(UT,Warning, TEXT("XMPGameMode: BeginPlay!"));
	// GEm: Make sure we start with at least 2 artifacts per node
	if (GoalScore <= 3)
		GoalScore = 4;

	// GEm: Destroy flag bases and spawn artifact nodes on top
	if (CTFGameState != nullptr)
	{
		for (i = 0; i < (*CTFGameState).FlagBases.Num(); i++)
		{
			if ((*(*CTFGameState).FlagBases[i]).MyFlag != nullptr)
			{
				//UE_LOG(UT,Warning, TEXT("XMPGameMode: Killing a flag!"));
				CTFGameState->FlagBases[i]->MyFlag->Destroy();
			}

			ArtifactNode = GetWorld()->SpawnActor<AXMPArtifactNode>((*(*CTFGameState).FlagBases[i]).GetActorLocation(), (*(*CTFGameState).FlagBases[i]).GetActorRotation());
			if (ArtifactNode)
			{
				//UE_LOG(UT,Warning, TEXT("XMPGameMode: Spawned a node!"));
				(*ArtifactNode).TeamNum = (*(*CTFGameState).FlagBases[i]).TeamNum;
				/*for (n = 0; n < (*(*CTFGameState).FlagBases[i]).TeamFlagTypes.Num(); n++)
				{
					(*ArtifactNode).TeamFlagTypes.Add((*(*CTFGameState).FlagBases[i]).TeamFlagTypes[n]);
				}*/
				//(*ArtifactNode).TeamFlagTypes = (*(*CTFGameState).FlagBases[i]).TeamFlagTypes;
				//UE_LOG(UT,Warning, TEXT("XMPGameMode: Killing a flag base!"));
				(*(*CTFGameState).FlagBases[i]).Destroy();
				//(*(*CTFGameState).FlagBases[i]).ConditionalBeginDestroy();
				(*CTFGameState).FlagBases[i] = ArtifactNode;
				//UE_LOG(UT,Warning, TEXT("XMPGameMode: Setting artifacts! Goal score: %d"), GoalScore);
				ArtifactNode->ArtifactNum = GoalScore / NumTeams;
				//UE_LOG(UT,Warning, TEXT("XMPGameMode: Initialising objectives! Artifacts in the node: %d"), ArtifactNode->ArtifactNum);
				(*ArtifactNode).InitializeObjective();
				GameObjectiveInitialized(ArtifactNode);
				//UE_LOG(UT,Warning, TEXT("XMPGameMode: We have a node!"));
			}
		}
	}

	Super::BeginPlay();

	// GEm: Team score shows artifacts in nodes (in U2XMP too!)
	for (i = 0; i < NumTeams; i++)
	{
		Teams[i]->Score = GoalScore / NumTeams;
	}
}

// GEm: Sync scores with artifact numbers (uses events, so might become desynced, maybe should go through artifacts instead)
void SELF::ScoreObject(AUTCarriedObject* GameObject, AUTCharacter* HolderPawn, AUTPlayerState* Holder, FName Reason)
{
	//UE_LOG(UT,Warning, TEXT("XMPGameMode: Score!!! Reason: %s Object: %d"), *Reason.ToString(), GameObject != nullptr);
	if (GameObject && GameObject->Team)
	{
		if (Reason == FName("ArtifactTaken"))
		{
			GameObject->Team->Score--;
			GameObject->Team->ForceNetUpdate();
		}
		else if (Reason == FName("ArtifactReset"))
		{
			GameObject->Team->Score++;
			GameObject->Team->ForceNetUpdate();
		}
	}
	Super::ScoreObject(GameObject, HolderPawn, Holder, Reason);
}

// GEm: Disable advantages for now (halftime not supported yet)
uint8 SELF::TeamWithAdvantage()
{
	return 255;
}

bool SELF::CheckAdvantage()
{
	return false;
}

#undef SELF
