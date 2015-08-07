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
#include "XPlayerController.h"
#include "UnrealNetwork.h"
#include "XHackTrigger.generated.h"

USTRUCT()
struct FUserInfo
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY()
	AXPlayerController* UserController;

	UPROPERTY()
	bool bUsing;

	UPROPERTY()
	float ProgressValue;
};

UCLASS()
class AXHackTrigger : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = HackTrigger)
	uint8 TeamNum;

	// Team at map load for resetting the map
	UPROPERTY()
	uint8 OriginalTeamNum;

	UPROPERTY()
	TArray<FUserInfo> Users;

	UPROPERTY()
	float TeamProgress[2];

	UPROPERTY()
	bool bIsOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HackTrigger)
	float TotalHackSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HackTrigger)
	float DecayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HackTrigger)
	bool bHackable;

	virtual void BeginPlay() override;

	void RemoveUser(AXPlayerController* User);

	void AddUser(AXPlayerController* User);

	void RemoveIdleUsers();

	bool AllowedToHack(AXPlayerController* User);

	int32 GetUserIndex(AXPlayerController* User);

	virtual void HandleHacked(int32 NewTeam);

	void DecayHackProcess(float DeltaTime);

	int32 UserCount(int32 TeamNum, bool bActiveUsers = false);

	void SetTeam(int32 TeamNum);

	bool ProgressExists();

	UFUNCTION(BlueprintCallable, Category = HackTrigger)
	float GetHackage(int32 NumValue);

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnUse(AXPlayerController* User);
	virtual void OnUnUse(AXPlayerController* User);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* DeniedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* StartHackSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* HackCompletedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* HackAmbientSound;

	UPROPERTY(BlueprintReadOnly, Category = Sound)
	USoundBase* CurrentAmbientSound;

	UPROPERTY(BlueprintReadOnly, Category = "Audio")
	UAudioComponent* AmbientSoundComp;

	UFUNCTION()
	void AmbientSoundUpdated();

	UFUNCTION()
	void SetAmbientSound();



};
