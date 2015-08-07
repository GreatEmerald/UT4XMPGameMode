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
#include "UTGameMode.h"
#include "XMPArtifactNode.h"
#include "XMPArtifact.h"

#define SELF AXMPArtifact

SELF::SELF(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bEnabled = true;
	// GEm: From the blueprint
	Holder3PSocketName = FName(TEXT("FlagAttach"));
	bTeamPickupSendsHome = true;
}

// GEm: Don't score when touching a flag, only when touching a base
bool SELF::CanBePickedUpBy(AUTCharacter* Character)
{
	return AUTCarriedObject::CanBePickedUpBy(Character);
}

// GEm: UnScore when taken
void SELF::SetHolder(AUTCharacter* NewHolder)
{
	//UE_LOG(UT,Warning, TEXT("XMPArtifact: I am team %d and a character from %d is trying to hold me!"), GetTeamNum(), NewHolder->GetTeamNum());
	Super::SetHolder(NewHolder);

	if (NewHolder == NULL || NewHolder->bPendingKillPending || NewHolder->PlayerState == NULL || Cast<AUTPlayerState>(NewHolder->PlayerState) == NULL)
		return;

	AUTGameMode* Game = GetWorld()->GetAuthGameMode<AUTGameMode>();

	if (ObjectState == CarriedObjectState::Held && Game)
		Game->ScoreObject(this, HoldingPawn, Holder, FName(TEXT("ArtifactTaken")));
}

// GEm: Get score back when reset
void SELF::SendHome()
{
	Super::SendHome();

	AUTGameMode* Game = GetWorld()->GetAuthGameMode<AUTGameMode>();

	if (Game)
		Game->ScoreObject(this, HoldingPawn, Holder, FName(TEXT("ArtifactReset")));
}

// GEm: Send flags back to base only if they are reset or returned, not captured (then it gets destroyed)
void SELF::Score(FName Reason, AUTCharacter* ScoringPawn, AUTPlayerState* ScoringPS)
{
	LastGameMessageTime = GetWorld()->GetTimeSeconds();
	AUTGameMode* Game = GetWorld()->GetAuthGameMode<AUTGameMode>();
	if (Game)
		Game->ScoreObject(this, ScoringPawn, ScoringPS, Reason);
	if (Reason == FName("SentHome"))
		SendHome();
}

// GEm: Unregister and die
void SELF::Deactivate()
{
	AXMPArtifactNode* AN = Cast<AXMPArtifactNode>(HomeBase);
	bool bSafeToDie;
	if (AN)
	{
		bSafeToDie = AN->UnregisterArtifact(this);
		bEnabled = false;
		// GEm: This is so bots don't crash as often. But SendHome() here makes the flag counts wrong at match end.
		if (bSafeToDie)
			Destroy();
		else
			SendHome();
	}
}


// Gem: Work around a bug
void SELF::OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetTeamNum() >= 255 || !bEnabled)
		return;
	Super::OnOverlapBegin(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

#undef SELF
