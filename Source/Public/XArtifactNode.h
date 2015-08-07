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

#include "XArtifactNode.generated.h"

class AXArtifact;
class AXCharacter;

UCLASS()
class AXArtifactNode : public AActor
{
	GENERATED_UCLASS_BODY()

	// The mesh for the flag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameObject)
	UStaticMeshComponent* Mesh;

	UStaticMeshComponent* GetMesh() const
	{
		return Mesh;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameObject)
	TArray<TSubclassOf<AXArtifact> > ArtifactClasses;

	UPROPERTY(BlueprintReadOnly, Category = GameObject)
	TArray<AXArtifact*> Artifacts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameObject)
	int32 TeamIndex;

	UPROPERTY()
	AXTeamInfo* Team;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameObject)
	TArray<FName> ArtifactSockets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameObject)
	FVector ArtifactOffset;

	UPROPERTY(Replicated)
		UMaterialInterface* Whatever;

	UFUNCTION()
	virtual void OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	virtual void BeginPlay() override;

	void GiveArtifactTo(AXCharacter* Stealer);

	UFUNCTION()
	void StoreArtifactFrom(AXCharacter* Donator);

	UFUNCTION()
	void StoreArtifact(AXArtifact* Artifact);




};
