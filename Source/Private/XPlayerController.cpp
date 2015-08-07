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
#include "XPlayerController.h"
#include "XEnergySource.h"
#include "XPlayerState.h"
#include "XDeployPoint.h"

AXPlayerController::AXPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UseCheckDistance = 200.f;
	bShowClassMenu = false;
	bIsInBehindView = false;
}

void AXPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("HoldSprint", IE_Pressed, this, &AXPlayerController::HoldSprint);
	InputComponent->BindAction("HoldSprint", IE_Released, this, &AXPlayerController::ReleaseSprint);
	InputComponent->BindAction("HoldJump", IE_Pressed, this, &AXPlayerController::HoldJump);
	InputComponent->BindAction("HoldJump", IE_Released, this, &AXPlayerController::ReleaseJump);

	InputComponent->BindAction("OnUse", IE_Released, this, &AXPlayerController::OnUse);
	InputComponent->BindAction("ThrowArtifact", IE_Released, this, &AXPlayerController::ThrowArtifact);

	InputComponent->BindAction("ToggleClassMenu", IE_Released, this, &AXPlayerController::ToggleClassMenu);
	InputComponent->BindAction("ToggleBehindView", IE_Released, this, &AXPlayerController::ToggleBehindView);

	XGameAnnouncer = NewObject<UXGameAnnouncer>(this, UXGameAnnouncer::StaticClass());



}

void AXPlayerController::SetPawn(APawn* InPawn){

	if (InPawn == NULL)
	{
		// Attempt to move the PC to the current camera location if no pawn was specified
		const FVector NewLocation = (PlayerCameraManager != NULL) ? PlayerCameraManager->GetCameraLocation() : GetSpawnLocation();
		SetSpawnLocation(NewLocation);
	}

	AUTPlayerController::SetPawn(InPawn);

	XCharacter = Cast<AXCharacter>(InPawn);

	if (Player && IsLocalPlayerController())
	{
		// apply FOV angle if dead/spectating
		if (GetPawn() == NULL && PlayerCameraManager != NULL)
		{
			FOV(ConfigDefaultFOV);
		}
		if (XCharacter && XCharacter->GetCharacterMovement())
		{
			//XCharacter->GetCharacterMovement()->UpdateSlideRoll(bIsHoldingSlideRoll);
			//SetAutoSlide(bAutoSlide);
			XCharacterMovement = Cast<UXCharacterMovement>(XCharacter->GetCharacterMovement());
		}
	}

}

void AXPlayerController::HoldSprint()
{
	if (XCharacterMovement == NULL) return;
	XCharacterMovement->bIsHoldingSprint = true;
}

void AXPlayerController::ReleaseSprint()
{
	if (XCharacterMovement == NULL) return;
	XCharacterMovement->bIsHoldingSprint = false;
}

void AXPlayerController::HoldJump()
{
	if (IsInState(FName(TEXT("NAME_WaitingToSpawn"))))
	{
		AXPlayerState* PS = Cast<AXPlayerState>(PlayerState);
		PS->bLoggedIn = !PS->bLoggedIn;
	}

	if (XCharacterMovement == NULL) return;
	else XCharacterMovement->bIsHoldingJump = true;
}

void AXPlayerController::ReleaseJump()
{
	if (XCharacterMovement == NULL) return;
	XCharacterMovement->bIsHoldingJump = false;
}

void AXPlayerController::OnUse()
{
	if (XCharacter == NULL) return;
	// UXCharacterMovement* MyCharMovement = XCharacter ? XCharacterMovement : NULL;
	// MyCharMovement->OnUse();
	if (Role != ROLE_Authority)
	{
		ServerOnUse();
	}

	FHitResult Result(1.f);
	static const FName UseCheckParamsTag = FName(TEXT("LandAssist"));
	FCollisionQueryParams CapsuleQuery(UseCheckParamsTag, false, XCharacter);
	FCollisionResponseParams ResponseParam;
	XCharacter->GetCharacterMovement()->InitCollisionParams(CapsuleQuery, ResponseParam);
	const FVector PawnLocation = XCharacter->GetCharacterMovement()->UpdatedComponent->GetComponentLocation();
	const ECollisionChannel CollisionChannel = XCharacter->GetCharacterMovement()->UpdatedComponent->GetCollisionObjectType();
	FRotator CharRotation = XCharacter->GetActorRotation();
	FVector Aim = FRotationMatrix(CharRotation).GetScaledAxis(EAxis::X);
	bool bHit = GetWorld()->SweepSingleByChannel(Result, PawnLocation, PawnLocation + Aim * UseCheckDistance, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(0.f), CapsuleQuery, ResponseParam);

	if (!Result.bBlockingHit)
	{
		return;
	}

	if (Result.GetActor() != NULL  && Result.GetActor()->IsA(AXEnergySource::StaticClass()))
	{
		AXEnergySource* HackTrigger = Cast<AXEnergySource>(Result.GetActor());
		UseTarget = HackTrigger;
		GetWorld()->GetTimerManager().SetTimer(CheckUseTimerHandle, this, &AXPlayerController::CheckUseTarget, 0.05f, true);
		bUsing = true;
		Cast<AXEnergySource>(UseTarget)->OnUse(this);
	}

	if (Result.GetActor() != NULL  && Result.GetActor()->IsA(AXDeployPoint::StaticClass()))
	{
		AXDeployPoint* HackTrigger = Cast<AXDeployPoint>(Result.GetActor());
		UseTarget = HackTrigger;
		GetWorld()->GetTimerManager().SetTimer(CheckUseTimerHandle, this, &AXPlayerController::CheckUseTarget, 0.05f, true);
		bUsing = true;
		Cast<AXDeployPoint>(UseTarget)->OnUse(this);
	}

}

void AXPlayerController::ServerOnUse_Implementation()
{
	OnUse();
}

void AXPlayerController::CheckUseTarget()
{
	FHitResult Result(1.f);
	static const FName UseCheckParamsTag = FName(TEXT("LandAssist"));
	FCollisionQueryParams CapsuleQuery(UseCheckParamsTag, false, XCharacter);
	FCollisionResponseParams ResponseParam;
	XCharacter->GetCharacterMovement()->InitCollisionParams(CapsuleQuery, ResponseParam);
	const FVector PawnLocation = XCharacter->GetCharacterMovement()->UpdatedComponent->GetComponentLocation();
	const ECollisionChannel CollisionChannel = XCharacter->GetCharacterMovement()->UpdatedComponent->GetCollisionObjectType();
	FRotator CharRotation = XCharacter->GetActorRotation();
	FVector Aim = FRotationMatrix(CharRotation).GetScaledAxis(EAxis::X);
	bool bHit = GetWorld()->SweepSingleByChannel(Result, PawnLocation, PawnLocation + Aim * UseCheckDistance, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(0.f), CapsuleQuery, ResponseParam);

	if (!Result.bBlockingHit)
	{
		if (UseTarget->IsA(AXHackTrigger::StaticClass()))
		{
			AXHackTrigger* Trigger = Cast<AXHackTrigger>(UseTarget);
			Trigger->OnUnUse(this);
			GetWorld()->GetTimerManager().ClearTimer(CheckUseTimerHandle);
			UseTarget = NULL;
			bUsing = false;
			XCharacter->SetHackEffect(false, FVector(0.f,0.f,0.f));
			return;
		}
	}

	if (Result.GetActor() != NULL && Result.GetActor() != UseTarget)
	{
		AXHackTrigger* Trigger = Cast<AXHackTrigger>(UseTarget);
		Trigger->OnUnUse(this);
		GetWorld()->GetTimerManager().ClearTimer(CheckUseTimerHandle);
		UseTarget = NULL;
		bUsing = false;
		XCharacter->SetHackEffect(false, FVector(0.f, 0.f, 0.f));
		return;
	}

	if (Result.GetActor() != NULL && Result.GetActor()->IsA(AXHackTrigger::StaticClass()) && Cast<AXHackTrigger>(Result.GetActor())->TeamNum == XCharacter->GetTeamNum())
	{
		AXHackTrigger* Trigger = Cast<AXHackTrigger>(UseTarget);
		Trigger->OnUnUse(this);
		GetWorld()->GetTimerManager().ClearTimer(CheckUseTimerHandle);
		UseTarget = NULL;
		bUsing = false;
		XCharacter->SetHackEffect(false, FVector(0.f, 0.f, 0.f));
		return;
	}

	if (Result.GetActor() != NULL && Result.GetActor()->IsA(AXHackTrigger::StaticClass()) && Cast<AXHackTrigger>(Result.GetActor())->bIsOn && Cast<AXHackTrigger>(Result.GetActor())->bHackable)
	{
		XCharacter->SetHackEffect(true, Result.Location);
	}

}

bool AXPlayerController::ServerOnUse_Validate()
{
	return true;
}

void AXPlayerController::ThrowArtifact()
{
	XCharacter->ThrowArtifact();
}

void AXPlayerController::SetNextDeployTime(float NewTime)
{
	NextDeployTime = GetWorld()->TimeSeconds + NewTime;
}

void AXPlayerController::GoToSpectateMode()
{
	AXPlayerState* PS = Cast<AXPlayerState>(PlayerState);
	if (PS != NULL)
	{
		PS->ServerRequestGoSpectate();
	}
}

void AXPlayerController::ToggleClassMenu()
{
	if (!bShowClassMenu && XHUD_Widget != NULL)
	{
		XHUD_Widget->ClassMenuCode->SetVisibility(ESlateVisibility::Visible);
		FInputModeGameAndUI InputMode;
		bShowMouseCursor = true;
		SetInputMode(InputMode);
	}
	else if (XHUD_Widget != NULL)
	{
		XHUD_Widget->ClassMenuCode->SetVisibility(ESlateVisibility::Hidden);
		FInputModeGameOnly InputMode;
		bShowMouseCursor = false;
		SetInputMode(InputMode);
	}
	bShowClassMenu = !bShowClassMenu;
}

void AXPlayerController::OnShowScores()
{
	ToggleScoreboard(true);
	bShowClassMenu = false;
	XHUD_Widget->SetVisibility(ESlateVisibility::Hidden);
}

void AXPlayerController::OnHideScores()
{
	ToggleScoreboard(false);
	XHUD_Widget->SetVisibility(ESlateVisibility::Visible);
}

void AXPlayerController::ToggleBehindView()
{
	bIsInBehindView = !bIsInBehindView;
	BehindView(bIsInBehindView);
}

void AXPlayerController::SetSpectatorPawn(class ASpectatorPawn* NewSpectatorPawn)
{
	Super::SetSpectatorPawn(NewSpectatorPawn);

	if (GetSpectatorPawn())
	{
		AXGameMode* Game = GetWorld()->GetAuthGameMode<AXGameMode>();
		int32 cameraint = FMath::RandRange(1, Game->MapCameras.Num());
		if (Game->MapCameras.Num()==0)return;
		ACameraActor* Camera = Game->MapCameras[cameraint-1];
		BehindView(false);
		bIsInBehindView = false;
		SetViewTarget(Camera);
		return;
	}
}

// TODO: I'm overwriting thos from UTPC as I see fit. If I run into errors I gotta look into this.
void AXPlayerController::OnFire()
{
	if (IsInState(NAME_Inactive))
	{
		ChangeState(FName(TEXT("NAME_WaitingToSpawn")));
		ServerViewNextRespawnPoint();
		return;
	}

	if (GetPawn() != NULL)
	{
		new(DeferredFireInputs)FDeferredFireInput(0, true);
	}

	else if (IsInState(NAME_Spectating))
	{
		ServerViewNextPlayer();
	}
	else if (IsInState(FName(TEXT("NAME_WaitingToSpawn"))))
	{
		ServerViewNextRespawnPoint();
	}

}

void AXPlayerController::OnAltFire()
{
	if (GetPawn() != NULL)
	{
		new(DeferredFireInputs)FDeferredFireInput(1, true);
	}

	else if (IsInState(NAME_Spectating))
	{
		ServerViewPrevPlayer();
	}
	else if (IsInState(FName(TEXT("NAME_WaitingToSpawn"))))
	{
		ServerViewPrevRespawnPoint();
	}
}


bool AXPlayerController::ServerViewNextRespawnPoint_Validate()
{
	return true;
}

void AXPlayerController::ServerViewNextRespawnPoint_Implementation()
{
	AXGameMode* Game = GetWorld()->GetAuthGameMode<AXGameMode>();
	RespawnPoint = Game->GetNextRespawnPoint(RespawnPoint,this);
	BehindView(false);
	bIsInBehindView = false;
	if (RespawnPoint == NULL || RespawnPoint->Camera == NULL) return;
	SetViewTarget(RespawnPoint->Camera);
	AXPlayerState* PS = Cast<AXPlayerState>(PlayerState);
	PS->RespawnPoint = RespawnPoint;
}

bool AXPlayerController::ServerViewPrevRespawnPoint_Validate()
{
	return true;
}

void AXPlayerController::ServerViewPrevRespawnPoint_Implementation()
{
	AXGameMode* Game = GetWorld()->GetAuthGameMode<AXGameMode>();
	RespawnPoint = Game->GetPreviousRespawnPoint(RespawnPoint,this);
	BehindView(false);
	bIsInBehindView = false;
	if (RespawnPoint == NULL || RespawnPoint->Camera == NULL) return;
	SetViewTarget(RespawnPoint->Camera);
	AXPlayerState* PS = Cast<AXPlayerState>(PlayerState);
	PS->RespawnPoint = RespawnPoint;
}
