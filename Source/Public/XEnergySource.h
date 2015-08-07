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

#include "XEnergySource.generated.h"

UCLASS()
class AXEnergySource : public AXHackTrigger
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameObject)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skin)
	UMaterialInterface* RedSkin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skin)
	UMaterialInterface* BlueSkin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skin)
	UMaterialInterface* NeutralSkin;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Skin)
	UMaterialInterface* CurrentSkin;

	UStaticMeshComponent* GetMesh() const
	{
		return Mesh;
	}

	UFUNCTION()
	virtual void OnRep_Skin();

	virtual void HandleHacked(int32 NewTeam) override;

	int32 EnergyRateBase;

	int32 EnergyRatePerPlayer;

	virtual void BeginPlay() override;

	void Activate();

	void Deactivate();

	bool bActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* EnergyAmbientSound;

	UPROPERTY(BlueprintReadOnly, Category = Sound)
	USoundBase* CurrentEnergyAmbientSound;

	UPROPERTY(BlueprintReadOnly, Category = "Audio")
	UAudioComponent* EnergyAmbientSoundComp;

	UFUNCTION()
	void EnergyAmbientSoundUpdated();


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif


};
