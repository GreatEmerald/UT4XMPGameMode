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
#pragma once

#include "XGameMode.h"
#include "UTCharacterContent.h"
#include "ComponentReregisterContext.h"
#include "XCharacter.generated.h"

class AXArtifact;

UCLASS(Config = Game)
class AXCharacter : public AUTCharacter
{
	GENERATED_UCLASS_BODY()

public:
	/** effect played on the character when the boots are activated */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	TSubclassOf<class AUTReplicatedEmitter> JumpJetEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking")
	float HackCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking")
	float HackRate;

	void ThrowArtifact();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking")
	UParticleSystemComponent* HackEffect;

	UPROPERTY()
	AXArtifact* CarriedArtifact;

	void SetArtifact(AXArtifact* Artifact);

	void StoreArtifact();

	virtual void ApplyCharacterData(TSubclassOf<class AUTCharacterContent> Data);

	/** Cached XCharacterMovement casted CharacterMovement */
	UPROPERTY(Category = Character, EditAnywhere, BlueprintReadWrite)
	class UXCharacterMovement* XCharacterMovement;

	void SetHackEffect(bool bActivate,FVector location);

};
