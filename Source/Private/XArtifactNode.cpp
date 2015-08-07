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

#include "XArtifactNode.h"
#include "XCharacter.h"
#include "XArtifact.h"
#include "XPlayerState.h"
#include "XPlayerController.h"
#include "XGameMessage.h"


AXArtifactNode::AXArtifactNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent, USceneComponent>(this, TEXT("DummyRoot"), false);
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Cube"));
	GetMesh()->AttachParent = RootComponent;

	GetMesh()->OnComponentBeginOverlap.AddDynamic(this, &AXArtifactNode::OnOverlapBegin);
	GetMesh()->bGenerateOverlapEvents = true;

	ArtifactOffset = FVector(0.f, 0.f, 300.f);

}

void AXArtifactNode::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AXArtifactNode, Whatever);
}

void AXArtifactNode::BeginPlay()
{
	for (int32 i = 0; i < ArtifactClasses.Num(); i++)
	{
		AXArtifact* NewArtifact = GetWorld()->SpawnActor<AXArtifact>(ArtifactClasses[i]);
		NewArtifact->DisablePhysics();
		FString socketname = "Position";
		socketname += FString::FromInt(i + 1);
		FName jesus = *socketname;
		NewArtifact->AttachRootComponentTo(GetMesh(), jesus, EAttachLocation::SnapToTarget);
		NewArtifact->TeamIndex = TeamIndex;
		Artifacts[i] = NewArtifact;
	}

	UStaticMeshComponent* TouchProxy = Cast<UStaticMeshComponent>(GetMesh()->GetChildComponent(0));
	TouchProxy->OnComponentBeginOverlap.AddDynamic(this, &AXArtifactNode::OnOverlapBegin);
	TouchProxy->bGenerateOverlapEvents = true;
}

void AXArtifactNode::OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AXCharacter* XCharacter = Cast<AXCharacter>(OtherActor);
	if (XCharacter != NULL)
	{
		AXPlayerState* XState = Cast<AXPlayerState>(XCharacter->PlayerState);

		if (TeamIndex != XCharacter->GetTeamNum() && !XState->bHasArtifact)
		{
			GiveArtifactTo(XCharacter);
		}
		else if (TeamIndex == XCharacter->GetTeamNum() && XState->bHasArtifact)
		{
			StoreArtifactFrom(XCharacter);
		}
	}

	AXArtifact* Artifact = Cast<AXArtifact>(OtherActor);
	if (Artifact != NULL)
	{
		if (Artifact->TeamIndex != TeamIndex)
		{
			StoreArtifact(Artifact);
		}
	}
}

void AXArtifactNode::StoreArtifact(AXArtifact* Artifact)
{
	Artifact->DisablePhysics();
	FString socketname = "Position";
	socketname += FString::FromInt(Artifacts.Num() + 1);
	FName jesus = *socketname;
	Artifact->AttachRootComponentTo(GetMesh(), jesus, EAttachLocation::SnapToTarget);
	Artifact->TeamIndex = TeamIndex;
	Artifacts.Add(Artifact);
	if (Artifacts.Num() >= 4)
	{
		AXGameMode* GM = GetWorld()->GetAuthGameMode<AXGameMode>();
		if (GM)
		{
			GM->BroadcastLocalized(this, UXGameMessage::StaticClass(), 19, NULL, NULL, this);
		}
	}
}

void AXArtifactNode::StoreArtifactFrom(AXCharacter* Donator)
{
	AXPlayerState* XState = Cast<AXPlayerState>(Donator->PlayerState);

	AXArtifact* Artifact = XState->CarriedArtifact;
	Artifact->DisablePhysics();
	FString socketname = "Position";
	socketname += FString::FromInt(Artifacts.Num() + 1);
	FName jesus = *socketname;
	Artifact->TeamIndex = TeamIndex;
	Artifact->AttachRootComponentTo(GetMesh(), jesus, EAttachLocation::SnapToTarget);
	Artifacts.Add(Artifact);
	Donator->StoreArtifact();
}

void AXArtifactNode::GiveArtifactTo(AXCharacter* Stealer)
{
	for (int32 i = 0; i < Artifacts.Num(); i++)
	{
		AXArtifact* Artifact = Artifacts[i];
		Artifact->DetachRootComponentFromParent();
		Artifact->AttachRootComponentTo(Stealer->GetMesh(), NAME_None, EAttachLocation::SnapToTarget);
		Artifact->GetMesh()->SetRelativeLocation(ArtifactOffset);
		Stealer->SetArtifact(Artifact);
		Artifacts.Remove(Artifact);

		AXPlayerController* PC = Cast<AXPlayerController>(Stealer->GetController());
		if (PC && (PC->PlayerState != NULL) && !PC->PlayerState->bOnlySpectator)
		{
			if (PC->XGameAnnouncer)
			{
				PC->ClientReceiveLocalizedMessage(UXGameMessage::StaticClass(), 0);
			}
		}

		return;
	}
}
