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

#include "XArtifact.h"

#include "XCharacter.h"


AXArtifact::AXArtifact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;
	PrimaryActorTick.bCanEverTick = true;

	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Cube"));
	RootComponent = GetMesh();

}

void AXArtifact::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AXArtifact, WhateverTest);
}

void AXArtifact::DisablePhysics()
{
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OnActorHit.RemoveDynamic(this, &AXArtifact::NotifyBump);
}

void AXArtifact::EnablePhysics()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);
	OnActorHit.AddDynamic(this, &AXArtifact::NotifyBump);
}

void AXArtifact::NotifyBump(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	AXCharacter* XCharacter = Cast<AXCharacter>(OtherActor);
	if (XCharacter == NULL) return;
	DisablePhysics();
	FString strong = GetRootComponent()->GetReadableName();
	UE_LOG(LogTemp, Warning, TEXT("rolf: %s"), *strong);

	AttachRootComponentTo(XCharacter->GetMesh(), NAME_None, EAttachLocation::SnapToTarget);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, 300.f));
	XCharacter->SetArtifact(this);
}
