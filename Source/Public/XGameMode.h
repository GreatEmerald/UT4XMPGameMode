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

#include "Core.h"
#include "UnrealTournament.h"
#include "Net/UnrealNetwork.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "XGameMode.generated.h"

class AXEnergySource;
class AXGameState;
class AXTeamInfo;
class AXDeployPoint;
class AXPlayerController;

UCLASS(Blueprintable, Abstract, Meta = (ChildCanTick), Config = XGameMode)
class AXGameMode : public AUTTeamGameMode
{
	GENERATED_UCLASS_BODY()

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void HandleMatchHasStarted() override;

	FTimerHandle EnergyTimerHandle;

	void UpdateTeamEnergy();

	void StoreEnergyFromSources();

	void NotifyEnergySourceExists(AXEnergySource* Source);

	TArray<AXEnergySource*> EnergySources;

	int32 NumEnergySources;

	UPROPERTY()
	AXGameState* XGameState;

	UPROPERTY(BlueprintReadOnly, Category = GameState)
	TArray<AXTeamInfo*> XTeams;

	/** class of TeamInfo to spawn */
	UPROPERTY(EditDefaultsOnly, Category = TeamGame)
	TSubclassOf<AXTeamInfo> XTeamClass;

	virtual void InitGameState() override;

	void CheckConsumerList();

	int32 GetAverageTeamSize();

	void DrainTeamEnergy(int TeamNum, float Amount);

	void AddDeployPoint(AXDeployPoint* DeployPoint);

	AXDeployPoint* DeployPointList;

	virtual void DefaultTimer() override;

	void Deploy();

	void DeployPlayers(AXDeployPoint* DeployPoint);

	bool IsReadyToDeploy(AXPlayerController* Controller, AXDeployPoint* DeployPoint);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameState)
	float RespawnDelaySeconds;

	void SendToSpectate(AXPlayerController* Controller);

	// virtual void StartNewPlayer(APlayerController* NewPlayer) override;

	virtual UClass* GetDefaultPawnClassForController(AController* InController);// override;

	TArray<ACameraActor*> MapCameras;

	virtual void StartPlay() override;

	AXDeployPoint* GetNextRespawnPoint(AXDeployPoint* PrevDP, AXPlayerController* C);
	AXDeployPoint* GetPreviousRespawnPoint(AXDeployPoint* NextDP, AXPlayerController* C);

	virtual class AActor* FindPlayerStart(AController* Player, const FString& IncomingName = TEXT(""));

};
