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

#include "XGameMessage.h"

#include "XEnergySource.h"

AXEnergySource::AXEnergySource(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	RedSkin = LoadObject<UMaterialInterface>(NULL, TEXT("/Game/RestrictedAssets/Environments/ShellResources/Materials/Vortex/G_Whitebox_Red.G_Whitebox_Red"));
	BlueSkin = LoadObject<UMaterialInterface>(NULL, TEXT("/Game/RestrictedAssets/Proto/UT3_Weapons/WP_Enforcers/Materials/M_EnforcerProto_Blue.M_EnforcerProto_Blue"));
	NeutralSkin = LoadObject<UMaterialInterface>(NULL, TEXT("/Game/RestrictedAssets/Blueprints/WIP/TomB/M_Glow_Toggle.M_Glow_Toggle"));
	CurrentSkin = NeutralSkin;


	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Cube"));
	RootComponent = GetMesh();

	EnergyRatePerPlayer = 1;
	EnergyRateBase = 3;
}

#if WITH_EDITOR
void AXEnergySource::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	OnRep_Skin();
}
#endif

void AXEnergySource::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AXEnergySource, CurrentSkin);
}

void AXEnergySource::BeginPlay()
{
	GetWorld()->GetAuthGameMode<AXGameMode>()->NotifyEnergySourceExists(this);
	OnRep_Skin();
	Activate();
}

void AXEnergySource::OnRep_Skin()
{
	if (TeamNum == 0)
	{
		CurrentSkin = RedSkin;
	}
	else if (TeamNum == 1)
	{
		CurrentSkin = BlueSkin;
	}
	else
	{
		CurrentSkin = NeutralSkin;
	}
	;
	for (int32 i = 0; i < Mesh->GetNumMaterials(); i++)
	{
		GetMesh()->SetMaterial(i, CurrentSkin);
	}
}

void AXEnergySource::HandleHacked(int32 NewTeam)
{
	Super::HandleHacked(NewTeam);
	OnRep_Skin();
	//ServerHacked();
	AXGameMode* GM = GetWorld()->GetAuthGameMode<AXGameMode>();
	if (GM)
	{
		GM->BroadcastLocalized(this, UXGameMessage::StaticClass(), 27, NULL, NULL, this);
	}
}

void AXEnergySource::Activate()
{
	bActive = true;
	CurrentEnergyAmbientSound = EnergyAmbientSound;
	EnergyAmbientSoundUpdated();
}

void AXEnergySource::Deactivate()
{
	bActive = false;
	CurrentEnergyAmbientSound = NULL;
	EnergyAmbientSoundUpdated();
}

void AXEnergySource::EnergyAmbientSoundUpdated()
{
	if (CurrentEnergyAmbientSound == NULL)
	{
		if (EnergyAmbientSoundComp != NULL)
		{
			EnergyAmbientSoundComp->Stop();
		}
	}
	else
	{
		if (EnergyAmbientSoundComp == NULL)
		{
			EnergyAmbientSoundComp = NewObject<UAudioComponent>(this);
			EnergyAmbientSoundComp->bAutoDestroy = false;
			EnergyAmbientSoundComp->bAutoActivate = false;
			EnergyAmbientSoundComp->AttachTo(RootComponent);
			EnergyAmbientSoundComp->RegisterComponent();
		}
		if (EnergyAmbientSoundComp->Sound != CurrentEnergyAmbientSound)
		{
			EnergyAmbientSoundComp->SetSound(CurrentEnergyAmbientSound);
		}
		if (!EnergyAmbientSoundComp->IsPlaying())
		{
			EnergyAmbientSoundComp->Play();
		}
	}
}


