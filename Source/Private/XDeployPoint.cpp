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

#include "XDeployPoint.h"

AXDeployPoint::AXDeployPoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;
	PrimaryActorTick.bCanEverTick = true;

	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("DeployPoint"));
	RootComponent = GetMesh();

	RedSkin = LoadObject<UMaterialInterface>(NULL, TEXT("/Game/RestrictedAssets/Environments/ShellResources/Materials/Vortex/G_Whitebox_Red.G_Whitebox_Red"));
	BlueSkin = LoadObject<UMaterialInterface>(NULL, TEXT("/Game/RestrictedAssets/Proto/UT3_Weapons/WP_Enforcers/Materials/M_EnforcerProto_Blue.M_EnforcerProto_Blue"));
	NeutralSkin = LoadObject<UMaterialInterface>(NULL, TEXT("/Game/RestrictedAssets/Blueprints/WIP/TomB/M_Glow_Toggle.M_Glow_Toggle"));
	CurrentSkin = NeutralSkin;
}

void AXDeployPoint::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AXDeployPoint, CurrentSkin);
}

#if WITH_EDITOR
void AXDeployPoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	OnRep_Skin();
}
#endif

void AXDeployPoint::BeginPlay()
{
	GetWorld()->GetAuthGameMode<AXGameMode>()->AddDeployPoint(this);
	OnRep_Skin();
}

void AXDeployPoint::OnRep_Skin()
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

void AXDeployPoint::HandleHacked(int32 NewTeam)
{
	Super::HandleHacked(NewTeam);
	OnRep_Skin();
	//ServerHacked();
}

bool AXDeployPoint::SetupDeployLocation()
{
	if (StartSpot) return true;
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.bNoCollisionFail = true;
	StartSpot = GetWorld()->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), GetActorLocation() + FVector(500.f, 500.f, 500.f), GetActorRotation(), Params);
	return true;
}


void AXDeployPoint::SetNextRespawnTime(AXDeployPoint* DeployPointList)
{
	AXDeployPoint* DP;
	AXDeployPoint* LatestDeployPoint = NULL;
	float MaxPctDifference = 0.f, CurrentPct = 0.f, CurrDiff = 0.f;

	for (DP = DeployPointList; DP != NULL; DP = DP->NextDeployPoint)
	{
		if (DP != this && DP->TeamNum == TeamNum && DP->RespawnDelayPct != RespawnDelayPct)
		{
			CurrentPct = DP->RespawnDelayPct;
			if (CurrentPct < RespawnDelayPct)
				CurrentPct += 1.f;

			CurrDiff = CurrentPct - RespawnDelayPct;
			if (CurrDiff > MaxPctDifference || LatestDeployPoint != NULL)
			{
				LatestDeployPoint = DP;
				MaxPctDifference = CurrDiff;
			}
		}
	}

	if (LatestDeployPoint == NULL)
	{
		LatestDeployPoint = this;
		MaxPctDifference = 0.f;
	}

	NextRespawnTime = LatestDeployPoint->NextRespawnTime + (1.f - MaxPctDifference) * GetWorld()->GetAuthGameMode<AXGameMode>()->RespawnDelaySeconds;
}
