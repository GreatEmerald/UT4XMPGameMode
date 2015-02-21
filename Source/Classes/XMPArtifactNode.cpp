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
#include "UTCTFFlag.h"
#include "XMPArtifact.h"
#include "XMPArtifactNode.h"

// GEm: class XMPArtifactNode extends UTCTFFlagBase;
#define SELF AXMPArtifactNode

// GEm: DefaultProperties
SELF::SELF(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TeamFlagTypes.Add(AXMPArtifact::StaticClass());
	TeamFlagTypes.Add(AXMPArtifact::StaticClass());
	// GEm: The number may be overridden by the gametype; maybe just use ArtifactIDs.Num()?
	ArtifactNum = 2;
	ArtifactIDs.Init(ArtifactNum);

	Collision = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("Capsule"));
	Collision->InitCapsuleSize(72.0f, 30.0f);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &SELF::OnOverlapBegin);
	Collision->SetCollisionProfileName(FName(TEXT("Pickup")));
	Collision->SetAbsolute(false, false, true);
	Collision->AttachParent = RootComponent;
}

// GEm: Spawn starting artifacts
void SELF::CreateCarriedObject()
{
	AUTCTFFlag* Artifact;
	int i;
	FActorSpawnParameters Params;
	Params.Owner = this;

	//UE_LOG(UT,Warning, TEXT("XMPArtifactNode: Creating artifacts!"));
	if (!TeamFlagTypes.IsValidIndex(TeamNum) || TeamFlagTypes[TeamNum] == NULL)
		return;

	for (i = 0; i < ArtifactNum; i++)
	{
		//UE_LOG(UT,Warning, TEXT("XMPArtifactNode: Creating an artifact!"));
		Artifact = (*GetWorld()).SpawnActor<AUTCTFFlag>(TeamFlagTypes[TeamNum], GetActorLocation() + FVector(0,0,96), GetActorRotation(), Params);
		if (Artifact)
		{
			ArtifactIDs[i] = Artifact;
			(*Artifact).Init(this);
		}
	}
	// GEm: For legacy (needed to determine base's team)
	UpdateLegacy();
	//UE_LOG(UT,Warning, TEXT("XMPArtifactNode: Team %d, CarriedObject %d, MyFlag %d"), TeamNum, CarriedObject != nullptr, MyFlag != nullptr);
}

// GEm: For HUD, will be unused when using custom HUD
FName SELF::GetFlagState()
{
	int i;
	FName Result = NAME_None;
	FName CurrentState;

	// GEm: Used for HUD, so get the "worst" state from all our artifacts
	for (i = 0; i < ArtifactIDs.Num(); i++)
	{
		if (ArtifactIDs[i] != nullptr)
		{
			CurrentState = ArtifactIDs[i]->ObjectState;
			if (Result == NAME_None
				|| (Result == CarriedObjectState::Home && (CurrentState == CarriedObjectState::Held || CurrentState == CarriedObjectState::Dropped))
				|| (Result == CarriedObjectState::Dropped && CurrentState == CarriedObjectState::Held))
				Result = CurrentState;
		}
	}
	return Result;
}

// GEm: Only to reduce crashes by the game not expecting multiple flags
void SELF::UpdateLegacy()
{
	if (ArtifactIDs.Num() > 0)
	{
		MyFlag = ArtifactIDs[0];
		CarriedObject = ArtifactIDs[0];
	}
	else
	{
		// GEm: Shouldn't actually happen, else crashes galore right now
		MyFlag = nullptr;
		CarriedObject = nullptr;
	}
}

bool SELF::UnregisterArtifact(AXMPArtifact* ArtifactID)
{
	if (ArtifactIDs.Num() <= 0)
		return false;

	ArtifactIDs.Remove(ArtifactID);
	UpdateLegacy();
	ArtifactNum--;
	return true;
}

void SELF::RegisterArtifact()
{
	int i;
	AUTCTFFlag* Artifact;
	FActorSpawnParameters Params;
	Params.Owner = this;

	ArtifactNum++;
	Artifact = (*GetWorld()).SpawnActor<AUTCTFFlag>(TeamFlagTypes[TeamNum], GetActorLocation() + FVector(0,0,96), GetActorRotation(), Params);
	if (Artifact)
	{
		ArtifactIDs.Add(Artifact);
		(*Artifact).Init(this);
		// GEm: If not for legacy reasons, could just call Destroy() on the old artifact, but now deactivate the last one
		for (i = 0; i < ArtifactIDs.Num(); i++)
		{
			if (!Cast<AXMPArtifact>(ArtifactIDs[i])->bEnabled)
				Cast<AXMPArtifact>(ArtifactIDs[i])->Deactivate();
		}
		UpdateLegacy();
	}
}

// GEm: The node now takes all the collision and scoring
void SELF::OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;

	AUTCharacter* Pawn = Cast<AUTCharacter>(OtherActor);
	if (!Pawn || !Pawn->GetCarriedObject())
		return;

	AXMPArtifact* Artifact = Cast<AXMPArtifact>(Pawn->GetCarriedObject());
	//UE_LOG(UT,Warning, TEXT("XMPArtifactNode: Overlap! Artifact: %d"), Artifact != nullptr);
	if (Artifact && Artifact->GetTeamNum() != TeamNum)
	{
		Artifact->Score(FName(TEXT("FlagCapture")), Artifact->HoldingPawn, Artifact->Holder);
		Artifact->Deactivate();
		RegisterArtifact();
	}
}

#undef SELF
