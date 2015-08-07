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
#include "XCharacter.h"
#include "XCharacterMovement.h"
#include "XHUD_Widget.h"
#include "XGameAnnouncer.h"
#include "XPlayerController.generated.h"

UCLASS(Config = Game)
class AXPlayerController : public AUTPlayerController
{
	GENERATED_UCLASS_BODY()

private:
	UPROPERTY()
	AXCharacter* XCharacter;

	UPROPERTY()
	UXCharacterMovement* XCharacterMovement;

public:
	virtual void SetupInputComponent() override;
	virtual void SetPawn(APawn* InPawn) override;

	FTimerHandle CheckUseTimerHandle;
	AActor* UseTarget;
	UPROPERTY(BlueprintReadOnly, Category = UseSetings)
	bool bUsing;

	UPROPERTY(BlueprintReadWrite, Category = HUD)
	UXHUD_Widget* XHUD_Widget;

	UPROPERTY(BlueprintReadWrite, Category = UseSetings)
		float UseCheckDistance;

	UFUNCTION()
		void CheckUseTarget();

	UFUNCTION()
		void ThrowArtifact();

	AXDeployPoint* RespawnPoint;

	void SetNextDeployTime(float NewTime);

	float NextDeployTime;

	UPROPERTY(BlueprintReadWrite, Category = UseSetings)
	bool bShowClassMenu;

	UFUNCTION(exec)
	void GoToSpectateMode();

	UFUNCTION()
	void ToggleClassMenu();

	virtual void OnShowScores() override;
	virtual void OnHideScores() override;

	bool bIsInBehindView;
	void ToggleBehindView();

	virtual void SetSpectatorPawn(class ASpectatorPawn* NewSpectatorPawn);

	void HoldSprint();
	void ReleaseSprint();
	void HoldJump();
	void ReleaseJump();
	void OnUse();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnUse();

	virtual void OnFire() override;
	virtual void OnAltFire() override;

	/** Move camera to previous player on round ended or spectating */
	UFUNCTION(unreliable, server, WithValidation)
	void ServerViewNextRespawnPoint();

	/** Move camera to previous player on round ended or spectating */
	UFUNCTION(unreliable, server, WithValidation)
	void ServerViewPrevRespawnPoint();

	UXGameAnnouncer* XGameAnnouncer;
};
