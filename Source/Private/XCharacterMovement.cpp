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
#include "XCharacterMovement.h"
#include "XCharacter.h"
#include "XHackTrigger.h"
#include "XEnergySource.h"
#include "UTReplicatedEmitter.h"

UXCharacterMovement::UXCharacterMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StaminaRechargeRate = 15.f;
	StaminaMax = 0.01f;
	Stamina = StaminaMax;
	SprintCost = 35.f;
	SprintRatio = 1.65f;
	SprintAccel = 6000.f;
	SprintSpeed = 950.f;
	MaxMultiJumpCount = 1;
	MultiJumpsRemaining = 1;
	JumpJetZ = 715.f;
	JumpJetCost = 0.003f;
	JumpJetDuration = 0.12f;
	DelayJumpJetTime = 0.15f;
	MantleCheckDistance = 50.f;
	UseCheckDistance = 200.f;
	SkillMax = 0.01f;
	Skill = SkillMax;
	SkillRechargeRate = 0.0007f;
}

float UXCharacterMovement::GetMaxSpeed() const
{
	float test = 13.f;
	if (bIsHoldingSprint && CanSprint())
	{
		test = MaxWalkSpeed * SprintRatio;
	}
	else
	{
		test = Super::GetMaxSpeed();
	}

	return test;
}

float UXCharacterMovement::GetMaxAcceleration() const
{
    if (bIsSprinting)
        return SprintAccel;
    return Super::GetMaxAcceleration();
}

bool UXCharacterMovement::CanSprint() const
{
	if (CharacterOwner && !IsCrouching() && Stamina > 0)
	{
		return true;
	}
	return false;
}

void UXCharacterMovement::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	UUTCharacterMovement::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsHoldingSprint && Stamina < StaminaMax)
	{
		Stamina += StaminaRechargeRate * 2 * DeltaTime;
		if (Stamina > StaminaMax)
		{
			Stamina = StaminaMax;
		}
	}
	if (bIsHoldingSprint && Velocity.Size2D() > 0.f)
	{
		Stamina -= DeltaTime * SprintCost;
		if (Stamina < 0)
		{
			Stamina = 0;
		}
	}

	bJumpJetting = JumpJetTimer > 0;
	if (bJumpJetting)
	{
		DoJumpJet(DeltaTime);
		JumpJetTimer -= DeltaTime;
	}

	if (Skill < SkillMax)
	{
		float AddedSkill = SkillRechargeRate * DeltaTime;
		if (Skill + AddedSkill > SkillMax)
		{
			AddedSkill = SkillMax - Skill;
		}
		else
		{
			Skill += AddedSkill;
		}
	}
}

void UXCharacterMovement::DoJumpJet(float DeltaTime)
{
	if (bJumpJetting)
	{
		Velocity.Z += JumpJetZ * FMath::Min(DeltaTime, JumpJetTimer) / JumpJetDuration;
	}
}

void UXCharacterMovement::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	// No breaking and deceleration in our game!
	UCharacterMovementComponent::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
}

bool UXCharacterMovement::DoMultiJump()
{
	if (CharacterOwner)
	{
		MultiJumpsRemaining -= 1;
		JumpJetTimer = JumpJetDuration;

		UseSkill(GetJumpJetCost());
		GetJumpJetCost();
		AUTCharacter* UTCharacterOwner = Cast<AUTCharacter>(CharacterOwner);
		if (UTCharacterOwner)
		{
			UUTGameplayStatics::UTPlaySound(GetWorld(), JumpJetSound, UTCharacterOwner, SRT_AllButOwner);
		}

		FActorSpawnParameters Params;
		AXCharacter* XCharOwner = Cast<AXCharacter>(CharacterOwner);
		Params.Owner = XCharOwner;
		Params.Instigator = XCharOwner;
		GetWorld()->SpawnActor<AUTReplicatedEmitter>(XCharOwner->JumpJetEffect, XCharOwner->GetActorLocation(), XCharOwner->GetActorRotation(), Params);


		if (CharacterOwner->IsA(AUTCharacter::StaticClass()))
		{
			static FName NAME_MultiJump(TEXT("MultiJump"));
			((AUTCharacter*)CharacterOwner)->InventoryEvent(NAME_MultiJump);
		}
		return true;
	}
	return false;
}

void UXCharacterMovement::UseSkill(float SkillUsed)
{
	if (CharacterOwner->Role == ROLE_Authority)
	{
		Skill -= SkillUsed;
	}
}

float UXCharacterMovement::GetJumpJetCost()
{
	if (bIsDodging)
	{
		return (JumpJetCost + JumpJetAfterDodgeCost) * 0.5;
	}
	else
	{
		return JumpJetCost * 0.5;
	}
}

bool UXCharacterMovement::DoJump(bool bReplayingMoves)
{
	Super::DoJump(bReplayingMoves);
	//JumpJetTimer = 0;
	DelayJumpJetTimer = DelayJumpJetTime;


	GetWorld()->GetTimerManager().SetTimer(MantleTimerHandle, this, &UXCharacterMovement::MantleTimer, 0.05f, true);

	return true;
}

void UXCharacterMovement::MantleTimer()
{
	if (!bIsHoldingJump)
	{
		GetWorld()->GetTimerManager().ClearTimer(MantleTimerHandle);
		return;
	}

	FHitResult Result(1.f);
	static const FName MantleCheckParamsTag = FName(TEXT("LandAssist"));
	FCollisionQueryParams CapsuleQuery(MantleCheckParamsTag, false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleQuery, ResponseParam);
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
	FRotator CharRotation = CharacterOwner->GetActorRotation();
	FVector Aim = FRotationMatrix(CharRotation).GetScaledAxis(EAxis::X);
	bool bHit = GetWorld()->SweepSingleByChannel(Result, PawnLocation, PawnLocation + Aim * MantleCheckDistance, FQuat::Identity, CollisionChannel, GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleQuery, ResponseParam);

	// DrawDebugLine(GetWorld(), PawnLocation, PawnLocation + Aim * MantleCheckDistance, FColor::Green, true);

	if (!Result.bBlockingHit)
	{
		return;
	}

	Velocity.Z = JumpZVelocity * 0.8f;

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Would mantle bro."));



}

bool UXCharacterMovement::CanMultiJump()
{
	return MultiJumpsRemaining > 0;
}

void UXCharacterMovement::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	Super::ProcessLanded(Hit, remainingTime, Iterations);

	MultiJumpsRemaining = MaxMultiJumpCount;
}

void UXCharacterMovement::OnUse()
{
	FHitResult Result(1.f);
	static const FName MantleCheckParamsTag = FName(TEXT("LandAssist"));
	FCollisionQueryParams CapsuleQuery(MantleCheckParamsTag, false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleQuery, ResponseParam);
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
	FRotator CharRotation = CharacterOwner->GetActorRotation();
	FVector Aim = FRotationMatrix(CharRotation).GetScaledAxis(EAxis::X);
	bool bHit = GetWorld()->SweepSingleByChannel(Result, PawnLocation, PawnLocation + Aim * UseCheckDistance, FQuat::Identity, CollisionChannel, GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleQuery, ResponseParam);

	if (!Result.bBlockingHit)
	{
		return;
	}


	if (Result.GetActor() != NULL  && Result.GetActor()->IsA(AXEnergySource::StaticClass()))
	{
		AXEnergySource* HackTrigger = Cast<AXEnergySource>(Result.GetActor());
		UseTarget = HackTrigger;
		GetWorld()->GetTimerManager().SetTimer(CheckUseTimerHandle, this, &UXCharacterMovement::CheckUseTarget, 0.05f, true);
		bUsing = true;
		Cast<AXEnergySource>(UseTarget)->OnUse(Cast<AXPlayerController>(CharacterOwner->GetController()));
	}

}

void UXCharacterMovement::CheckUseTarget()
{
	FHitResult Result(1.f);
	static const FName MantleCheckParamsTag = FName(TEXT("LandAssist"));
	FCollisionQueryParams CapsuleQuery(MantleCheckParamsTag, false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleQuery, ResponseParam);
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
	FRotator CharRotation = CharacterOwner->GetActorRotation();
	FVector Aim = FRotationMatrix(CharRotation).GetScaledAxis(EAxis::X);
	bool bHit = GetWorld()->SweepSingleByChannel(Result, PawnLocation, PawnLocation + Aim * UseCheckDistance, FQuat::Identity, CollisionChannel, GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleQuery, ResponseParam);

	if (!Result.bBlockingHit)
	{
		AXEnergySource* Source = Cast<AXEnergySource>(UseTarget);
		Source->OnUnUse(Cast<AXPlayerController>(CharacterOwner->GetController()));
		GetWorld()->GetTimerManager().ClearTimer(CheckUseTimerHandle);
		UseTarget = NULL;
		bUsing = false;
		//Disable hack effect
		return;
	}

	if (Result.GetActor() != NULL && Result.GetActor()->IsA(AXEnergySource::StaticClass()) && Cast<AXEnergySource>(Result.GetActor())->bIsOn)
	{
		// Spawn a hack effect
	}

	if (Result.GetActor() != NULL && Result.GetActor() != UseTarget)
	{
		AXEnergySource* Source = Cast<AXEnergySource>(UseTarget);
		Source->OnUnUse(Cast<AXPlayerController>(CharacterOwner->GetController()));
		GetWorld()->GetTimerManager().ClearTimer(CheckUseTimerHandle);
		UseTarget = NULL;
		bUsing = false;
		//Disable hack effect
	}

	AXCharacter* XCharOwner = Cast<AXCharacter>(CharacterOwner);
	if (Result.GetActor() != NULL && Result.GetActor()->IsA(AXEnergySource::StaticClass()) && Cast<AXEnergySource>(Result.GetActor())->TeamNum == XCharOwner->GetTeamNum())
	{
		bUsing = false;
		// Spawn a hack effect
	}



}

bool UXCharacterMovement::HasNeededSkill(float SkillTest)
{
	return (SkillTest <= 0 || this->Skill >= SkillTest);
}
