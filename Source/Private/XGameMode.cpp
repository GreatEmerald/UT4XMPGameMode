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
#include "XGameState.h"
#include "XPlayerState.h"
#include "XEnergySource.h"
#include "XTeamInfo.h"
#include "XDeployPoint.h"
#include "UTScoreboard.h"
#include "XEndoCharacter.h"
#include "XEctoCharacter.h"

AXGameMode::AXGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AXGameState::StaticClass();
	PlayerStateClass = AXPlayerState::StaticClass();
	RespawnDelaySeconds = 6;
	bStartPlayersAsSpectators = true;
}

void AXGameMode::InitGameState()
{
	Super::InitGameState();

	for (int32 i = 0; i <= 15; i++)
	{
		EnergySources.Add(NULL);
	}

}

void AXGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{

	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		ACameraActor* Camera = Cast<ACameraActor>(*It);
		if (!Camera) return;
		if (Camera->ActorHasTag(FName(TEXT("MapCamera"))))
		{
			MapCameras.Add(Camera);
		}
	}

	AUTGameMode::InitGame(MapName, Options, ErrorMessage);

	bBalanceTeams = EvalBoolOptions(ParseOption(Options, TEXT("BalanceTeams")), bBalanceTeams);

	if (bAllowURLTeamCountOverride)
	{
		NumTeams = GetIntOption(Options, TEXT("NumTeams"), NumTeams);
	}
	NumTeams = FMath::Max<uint8>(NumTeams, 2);

	if (XTeamClass == NULL)
	{
		XTeamClass = AXTeamInfo::StaticClass();
	}
	for (uint8 i = 0; i < NumTeams; i++)
	{
		AXTeamInfo* NewTeam = GetWorld()->SpawnActor<AXTeamInfo>(XTeamClass);
		NewTeam->TeamIndex = i;
		if (TeamColors.IsValidIndex(i))
		{
			NewTeam->TeamColor = TeamColors[i];
		}

		if (TeamNames.IsValidIndex(i))
		{
			NewTeam->TeamName = TeamNames[i];
		}

		Teams.Add(NewTeam);
		checkSlow(Teams[i] == NewTeam);
	}
	XTeams.Add(Cast<AXTeamInfo>(Teams[0]));
	XTeams.Add(Cast<AXTeamInfo>(Teams[1]));

	MercyScore = FMath::Max(0, GetIntOption(Options, TEXT("MercyScore"), MercyScore));

	// TDM never kills off players going in to overtime
	bOnlyTheStrongSurvive = false;



}

void AXGameMode::StartPlay()
{
	Super::StartPlay();


}

void AXGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	GetWorld()->GetTimerManager().SetTimer(EnergyTimerHandle, this, &AXGameMode::UpdateTeamEnergy, 1.f, true);
}

void AXGameMode::UpdateTeamEnergy()
{
	StoreEnergyFromSources();
	CheckConsumerList();

}

void AXGameMode::CheckConsumerList()
{
	DrainTeamEnergy(0, 3.f);
	DrainTeamEnergy(1, 3.f);
}

void AXGameMode::DrainTeamEnergy(int TeamNum, float Amount)
{
	XTeams[TeamNum]->TeamEnergy -= Amount;
}

int32 AXGameMode::GetAverageTeamSize()
{
	// Need this later on to scale draining of energy etc.
	return 1;
}

void AXGameMode::StoreEnergyFromSources()
{
	for (int32 i = 0; i < EnergySources.Num(); i++)
	{
		if (EnergySources[i] != NULL)
		{
			AXEnergySource* ES = EnergySources[i];
			int8 ESTeam = ES->TeamNum;
			if (ESTeam != 1 || ESTeam != 0) continue;
			if (NumEnergySources > 0)
			{
				int32 Sum = ES->EnergyRateBase + ES->EnergyRatePerPlayer;
				XTeams[ESTeam]->TeamEnergy += Sum;
			}
		}
	}

}

// sources existieren noch nicht! <<<<<<<<<<<<<-----------
void AXGameMode::NotifyEnergySourceExists(AXEnergySource* Source)
{
	for (int32 i = 0; i <= EnergySources.Num(); i++)
	{
		if (EnergySources[i] == NULL)
		{
			EnergySources[i] = Source;
			NumEnergySources++;
			break;
		}
	}
}

void AXGameMode::AddDeployPoint(AXDeployPoint* DeployPoint)
{
	DeployPoint->NextDeployPoint = DeployPointList;
	DeployPointList = DeployPoint;
}


void AXGameMode::DefaultTimer()
{
	Super::DefaultTimer();
	Deploy();
}

void AXGameMode::Deploy()
{
	for (AXDeployPoint* DP = DeployPointList; DP != NULL; DP = DP->NextDeployPoint)
	{
		if (DP->TeamNum == 255) continue;
		if (DP->NextRespawnTime <= GetWorld()->TimeSeconds)
		{
			DeployPlayers(DP);
			DP->SetNextRespawnTime(DeployPointList);

			for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
			{
				AXPlayerController* C = Cast<AXPlayerController>(It->Get());
				if (C != NULL && C->RespawnPoint == DP)
				{
					C->SetNextDeployTime(DP->NextRespawnTime - GetWorld()->TimeSeconds);
				}
			}
		}
	}
}

void AXGameMode::DeployPlayers(AXDeployPoint* DeployPoint)
{
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AXPlayerController* C = Cast<AXPlayerController>(It->Get());
		if (IsReadyToDeploy(C, DeployPoint) && DeployPoint->SetupDeployLocation())
		{
			RestartPlayer(C);
			if (C->GetPawn() != NULL)
			{
				AXPlayerState* PS = Cast<AXPlayerState>(C->PlayerState);
				PS->bLoggedIn = false;
				C->GetPawn()->SetActorRotation(C->GetControlRotation());
			}
		}
	}
}

AActor* AXGameMode::FindPlayerStart(AController* Player, const FString& IncomingName)
{
	AXPlayerController* C = Cast<AXPlayerController>(Player);
	if (C->RespawnPoint) return C->RespawnPoint->StartSpot;
	return Super::FindPlayerStart(Player, IncomingName);
}

bool AXGameMode::IsReadyToDeploy(AXPlayerController* Controller, AXDeployPoint* DeployPoint)
{
	AXPlayerState* PS = Cast<AXPlayerState>(Controller->PlayerState);
	return (Controller->PlayerState != NULL && PS->bLoggedIn && PS->RespawnPoint == DeployPoint);
}

void AXGameMode::SendToSpectate(AXPlayerController* Controller)
{
	if (Controller->GetPawn() != NULL)
	{
		AXCharacter* XChar = Cast<AXCharacter>(Controller->GetPawn());
		if (XChar != NULL)
		{
			XChar->Died(NULL, FDamageEvent(UUTDamageType::StaticClass()));
		}
	}
	Controller->ChangeState(NAME_Spectating);
}

/*void AXGameMode::StartNewPlayer(APlayerController* NewPlayer)
{
AXPlayerController* XNewPlayer = Cast<AXPlayerController>(NewPlayer);
if (XNewPlayer != NULL)
{
// tell client what hud class to use
TSubclassOf<UUTScoreboard> ScoreboardClass = LoadClass<UUTScoreboard>(NULL, *ScoreboardClassName.AssetLongPathname, NULL, LOAD_None, NULL);
XNewPlayer->ClientSetHUDAndScoreboard(HUDClass, ScoreboardClass);

// After a guy connects, we first send him to spectate. Then he can decide on team and class after
// looking at the scoreboard etc. to make is OWN decisions...
SendToSpectate(XNewPlayer);



// start match, or let player enter, immediately
if (XGameState->HasMatchStarted())
{
RestartPlayer(NewPlayer);
}

if (NewPlayer->GetPawn() != NULL)
{
NewPlayer->GetPawn()->ClientSetRotation(NewPlayer->GetPawn()->GetActorRotation());
}

}
}*/

UClass* AXGameMode::GetDefaultPawnClassForController(AController* InController)
{
	AXPlayerController* XController = Cast<AXPlayerController>(InController);
	if (XController->XHUD_Widget->SelectedClass.Equals("Endo"))
	{
		TSubclassOf<AXEndoCharacter> XClassOne = LoadClass<AXEndoCharacter>(NULL, TEXT("/XGameMode/BP_XEndoCharacter.BP_XEndoCharacter_C"), NULL, LOAD_None, NULL);
		return XClassOne;
	}
	else
	{
		TSubclassOf<AXEctoCharacter> XClassTwo = LoadClass<AXEctoCharacter>(NULL, TEXT("/XGameMode/BP_XEctoCharacter.BP_XEctoCharacter_C"), NULL, LOAD_None, NULL);
		return XClassTwo;
	}
}

AXDeployPoint* AXGameMode::GetNextRespawnPoint(AXDeployPoint* PrevDP, AXPlayerController* C)
{
	AXDeployPoint* DP = NULL;
	if (PrevDP)
	{
		DP = PrevDP->NextDeployPoint;
	}
	if (!DP)
	{
		DP = DeployPointList;
	}

	int8 overflow = 0;

	if (C)
	{
		while (overflow < 2 && DP->TeamNum != C->GetTeamNum())
		{
			DP = DP->NextDeployPoint;
			if (!DP)
			{
				DP = DeployPointList;
				overflow++;
			}
		}
		if (DP != NULL && (DP->TeamNum != C->GetTeamNum()))
		{
			return NULL;
		}
	}
	return DP;
}

AXDeployPoint* AXGameMode::GetPreviousRespawnPoint(AXDeployPoint* NextDP, AXPlayerController* C)
{
	AXDeployPoint* DP;
	AXDeployPoint* candidate;
	bool changed = false;

	if (!NextDP) return DeployPointList;

	DP = DeployPointList;
	candidate = DP;
	while (DP->NextDeployPoint != NULL && DP != NextDP)
	{
		if (DP->TeamNum == C->GetTeamNum())
		{
			candidate = DP;
			changed = true;
		}
		DP = DP->NextDeployPoint;
	}

	if (candidate == DeployPointList && !changed)
	{
		DP = NextDP;
		while (DP != NULL)
		{
			if (DP->TeamNum == C->GetTeamNum())
			{
				candidate = DP;
				changed = true;
			}
			DP = DP->NextDeployPoint;
		}
	}
	return candidate;

}
