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

#include "UTCharacterMovement.h"

#include "XCharacterMovement.generated.h"

UCLASS()
class UXCharacterMovement : public UUTCharacterMovement
{
	GENERATED_UCLASS_BODY()

public:

	/** Return SprintSpeed if CanSprint(). */
	virtual float GetMaxSpeed() const override;

	/** Support for sprint acceleration. */
	virtual float GetMaxAcceleration() const override;

	/** Return true if character can sprint right now */
	virtual bool CanSprint() const override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/** Stamina and Sprint properties */
	UPROPERTY(Category = "ActiveSprint", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Stamina"))
	float Stamina;

	UPROPERTY(Category = "ActiveSprint", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Max Stamina"))
	float StaminaMax;

	UPROPERTY(Category = "ActiveSprint", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Stamina Recharge Rate"))
	float StaminaRechargeRate;

	UPROPERTY(Category = "ActiveSprint", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Sprint Cost"))
	float SprintCost;

	UPROPERTY(Category = "ActiveSprint", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Sprint Ratio"))
	float SprintRatio;

	UPROPERTY()
	bool bIsHoldingSprint;

	UPROPERTY()
	bool bIsHoldingJump;

	/** Jump Jets*/
	UPROPERTY()
	bool bJumpJetting;

	UPROPERTY(Category = "JumpJets", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Jump Jet Z"))
	float JumpJetZ;

	UPROPERTY()
	float JumpJetCost;

	UPROPERTY()
	float JumpJetAfterDodgeCost;

	UPROPERTY(Category = "JumpJets", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Jump Jet Duration"))
	float JumpJetDuration;

	UPROPERTY()
	float JumpJetTimer;

	UPROPERTY()
	float DelayJumpJetTime;

	UPROPERTY()
	float DelayJumpJetTimer;

	UPROPERTY()
	int32 MultiJumpsRemaining;

	/** Skill */
	UPROPERTY(Category = "Skill", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Skill"))
	float Skill;

	UPROPERTY(Category = "Skill", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Skill"))
	float SkillMax;

	UPROPERTY(Category = "Skill", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Skill"))
	float SkillRechargeRate;

	virtual void UseSkill(float SkillUsed);

	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;

	/** Perform a multijump */
	virtual bool DoMultiJump() override;

	virtual float GetJumpJetCost();

	virtual void DoJumpJet(float DeltaTime);

	virtual bool DoJump(bool bReplayingMoves) override;

	/** Return true if can multijump now. */
	virtual bool CanMultiJump() override;

	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;

	void MantleTimer();

	void OnUse();

	float MantleCheckDistance;

	FTimerHandle MantleTimerHandle;

	FTimerHandle CheckUseTimerHandle;

	UPROPERTY(BlueprintReadWrite, Category = UseSetings)
	float UseCheckDistance;

	UFUNCTION()
	bool HasNeededSkill(float Amount);

	AActor* UseTarget;

	UFUNCTION()
	void CheckUseTarget();

	UPROPERTY(BlueprintReadOnly, Category = UseSetings)
	bool bUsing;

	/** SOUNDS */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sounds)
	USoundBase* JumpJetSound;
};
