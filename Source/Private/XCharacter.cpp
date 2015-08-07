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
#include "XCharacter.h"
#include "XCharacterMovement.h"
#include "XArtifact.h"
#include "XPlayerState.h"
#include "XPlayerController.h"


AXCharacter::AXCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UXCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	UTCharacterMovement = Cast<UXCharacterMovement>(GetCharacterMovement());
	XCharacterMovement = Cast<UXCharacterMovement>(GetCharacterMovement());

	HackRate = 0.3f;
	HackCost = 0.005f;
}

void AXCharacter::SetArtifact(AXArtifact* Artifact)
{
	CarriedArtifact = Artifact;
	AXPlayerState* XState = Cast<AXPlayerState>(PlayerState);
	XState->bHasArtifact = true;
	XState->CarriedArtifact = Artifact;
}

void AXCharacter::ThrowArtifact()
{
	if (CarriedArtifact == NULL) return;

	CarriedArtifact->DetachRootComponentFromParent();
	CarriedArtifact->EnablePhysics();

	FVector Impulse =  GetVelocity();
	Impulse.Z = 600;

	CarriedArtifact->GetMesh()->AddImpulse(Impulse,NAME_None,true);
	CarriedArtifact->GetMesh()->AddAngularImpulse(FVector(20.f, 20.f, 20.f),NAME_None, true);
	CarriedArtifact = NULL;
	AXPlayerState* XState = Cast<AXPlayerState>(PlayerState);
	XState->bHasArtifact = false;
	XState->CarriedArtifact = NULL;
}

void AXCharacter::StoreArtifact()
{
	AXPlayerState* XState = Cast<AXPlayerState>(PlayerState);
	XState->bHasArtifact = false;
	XState->CarriedArtifact = NULL;
	CarriedArtifact = NULL;
}

void AXCharacter::ApplyCharacterData(TSubclassOf<AUTCharacterContent> CharType)
{
}

void AXCharacter::SetHackEffect(bool bActivate, FVector TargetLocation)
{
	if (bActivate)
	{
		static FName NAME_HitLocation(TEXT("HitLocation"));
		HackEffect->SetVectorParameter(NAME_HitLocation, TargetLocation);
		HackEffect->ActivateSystem();
	}
	else
	{
		HackEffect->DeactivateSystem();
	}
}
