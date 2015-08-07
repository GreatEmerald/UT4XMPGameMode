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

#include "XHackTrigger.h"

AXHackTrigger::AXHackTrigger(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TotalHackSeconds = 3.f;
	DecayRate = 0.4f;
	bHackable = true;

	bReplicates = true;
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;

	PrimaryActorTick.bCanEverTick = true;

	TeamNum = 255;
	bIsOn = true;
}

void AXHackTrigger::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AXHackTrigger, TeamNum);
}

void AXHackTrigger::BeginPlay()
{
	OriginalTeamNum = TeamNum;
}

void AXHackTrigger::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	for (int32 i = 0; i < Users.Num(); i++)
	{
		if (!Users[i].bUsing) continue;
		AXCharacter* XCharacter = Cast<AXCharacter>(Users[i].UserController->GetPawn());
		UXCharacterMovement* XCharacterMovement = Cast<UXCharacterMovement>(XCharacter->GetCharacterMovement());
		int32 UserTeam = XCharacter->GetTeamNum();
		if (UserTeam != TeamNum)
		{
			float NeededSkill = XCharacter->HackCost * XCharacter->HackRate * DeltaSeconds;
			if (XCharacterMovement->HasNeededSkill(NeededSkill))
			{
				float Hackage = XCharacter->HackRate * DeltaSeconds;
				TeamProgress[UserTeam] += Hackage;
				Users[i].ProgressValue += Hackage;
				XCharacterMovement->UseSkill(NeededSkill);
			}
		}
	}

	if (Users.Num()>0)
	{
		for (int32 i = 0; i < ARRAY_COUNT(TeamProgress); i++)
		{
			if (TeamProgress[i]>TotalHackSeconds)
			{
				HandleHacked(i);
			}
		}
	}

	DecayHackProcess(DeltaSeconds);
	RemoveIdleUsers();

	if (UserCount(0, true) + UserCount(1, true) == 0)
	{
		CurrentAmbientSound = NULL;
		AmbientSoundUpdated();
	}

	if (!ProgressExists())
	{
		SetActorTickEnabled(false);
	}

}

bool AXHackTrigger::ProgressExists()
{
	for (int32 i = 0; i < ARRAY_COUNT(TeamProgress); i++)
	{
		if (TeamProgress[i] > 0)
		{
			return true;
		}
	}
	return false;
}

void AXHackTrigger::OnUse(AXPlayerController* User)
{
	//ServerOnUse(User);

	if (AllowedToHack(User) && bHackable)
	{
		AddUser(User);
		CurrentAmbientSound = HackAmbientSound;
		SetActorTickEnabled(true);
		AmbientSoundUpdated();
		UUTGameplayStatics::UTPlaySound(GetWorld(), StartHackSound, this, SRT_None);


	}
	else
	{
		UUTGameplayStatics::UTPlaySound(GetWorld(), DeniedSound, this, SRT_None);
	}
}

void AXHackTrigger::AmbientSoundUpdated()
{
	if (CurrentAmbientSound == NULL)
	{
		if (AmbientSoundComp != NULL)
		{
			AmbientSoundComp->Stop();
		}
	}
	else
	{
		if (AmbientSoundComp == NULL)
		{
			AmbientSoundComp = NewObject<UAudioComponent>(this);
			AmbientSoundComp->bAutoDestroy = false;
			AmbientSoundComp->bAutoActivate = false;
			AmbientSoundComp->AttachTo(RootComponent);
			AmbientSoundComp->RegisterComponent();
		}
		if (AmbientSoundComp->Sound != CurrentAmbientSound)
		{
			AmbientSoundComp->SetSound(CurrentAmbientSound);
		}
		if (!AmbientSoundComp->IsPlaying())
		{
			AmbientSoundComp->Play();
		}
	}
}

void AXHackTrigger::SetAmbientSound()
{
	//HackAmbientSound = HackAmbientSound;
}

void AXHackTrigger::OnUnUse(AXPlayerController* User)
{
	if (Role < ROLE_Authority)
	{
		//ServerOnUnUse(User);
	}

	RemoveUser(User);
}

void AXHackTrigger::RemoveUser(AXPlayerController* User)
{
	int32 UserIndex = GetUserIndex(User);
	if (UserIndex >= 0)
	{
		Users[UserIndex].bUsing = false;
	}

	if (User->PlayerState != NULL && !User->PlayerState->bOnlySpectator)
	{
		if (User->XGameAnnouncer)
		{
			User->ClientReceiveLocalizedMessage(UXGameMessage::StaticClass(), 1);
		}
	}
}

bool AXHackTrigger::AllowedToHack(AXPlayerController* User)
{
	return User->GetTeamNum() != TeamNum;
}

int32 AXHackTrigger::GetUserIndex(AXPlayerController* User)
{
	for (int32 i = 0; i < Users.Num(); i++)
	{
		if (Users[i].UserController == User)
		{
			return i;
		}
	}
	return -1;
}

void AXHackTrigger::AddUser(AXPlayerController* User)
{
	int32 Index = 0;
	if (GetUserIndex(User) < 0)
	{
		FUserInfo NewUser;
		NewUser.UserController = User;
		Users.Add(NewUser);
		Index = 0;
	}
	Users[Index].bUsing = true;
}

void AXHackTrigger::RemoveIdleUsers()
{
	for (int32 i = 0; i < Users.Num(); i++)
	{
		if (!Users[i].bUsing && Users[i].ProgressValue <= 0)
		{
			Users.RemoveAt(i);
		}
	}
}

void AXHackTrigger::HandleHacked(int32 NewTeam)
{
	SetTeam(NewTeam);

	for (int32 i = 0; i < ARRAY_COUNT(TeamProgress); i++)
	{
		TeamProgress[i] = 0;
	}
	UUTGameplayStatics::UTPlaySound(GetWorld(), HackCompletedSound, this, SRT_None);

	CurrentAmbientSound = NULL;
	AmbientSoundUpdated();
}


void AXHackTrigger::SetTeam(int32 NewTeam)
{
	if (Role == ROLE_Authority)
	{
		this->TeamNum = NewTeam;
	}
}

void AXHackTrigger::DecayHackProcess(float DeltaTime)
{
	for (int32 i = 0; i < ARRAY_COUNT(TeamProgress); i++)
	{
		if (TeamProgress[i]>0 && UserCount(i, true) == 0)
		{
			if (UserCount(i) > 0)
			{
				for (int32 u = 0; u < Users.Num(); u++)
				{
					AXPlayerController* XController = Users[u].UserController;
					if (XController->GetTeamNum() == TeamNum)
					{
						Users[u].ProgressValue = FMath::Max<float>(0.f, Users[u].ProgressValue - DecayRate * DeltaTime / UserCount(u));
					}
				}
			}
			TeamProgress[i] = FMath::Max<float>(0.f, TeamProgress[i] - DecayRate * DeltaTime);
		}
	}
}

int32 AXHackTrigger::UserCount(int32 Team, bool bActiveUsers)
{
	int32 TeamCount = 0;
	for (int32 i = 0; i < Users.Num(); i++)
	{
		AXPlayerController* XController = Users[i].UserController;
		if (XController->GetTeamNum() == Team)
		{
			if (Users[i].bUsing || !bActiveUsers)
			{
				TeamCount++;
			}
		}
	}
	return TeamCount;
}

float AXHackTrigger::GetHackage(int32 Team)
{
	return TeamProgress[Team];
}
