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

#include "XHackTrigger.h"

#include "XDeployPoint.generated.h"

UCLASS()
class AXDeployPoint : public AXHackTrigger
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UStaticMeshComponent* GetMesh() const
	{
		return Mesh;
	}

	APlayerStart* StartSpot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CylSkin)
	UMaterialInterface* RedSkin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CylSkin)
	UMaterialInterface* BlueSkin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CylSkin)
	UMaterialInterface* NeutralSkin;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Skin)
	UMaterialInterface* CurrentSkin;

	virtual void HandleHacked(int32 NewTeam) override;

	virtual void BeginPlay() override;

	AXDeployPoint* NextDeployPoint;

	float NextRespawnTime;

	void SetNextRespawnTime(AXDeployPoint* DeployPointList);

	float RespawnDelayPct;

	bool SetupDeployLocation();

	UFUNCTION()
	virtual void OnRep_Skin();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// Camera set by the mapper that is showing the respawnpoint and its surrounding area.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameObject)
	ACameraActor* Camera;

};
