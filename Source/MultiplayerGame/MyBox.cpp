// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBox.h"
#include <Net/UnrealNetwork.h>
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyBox::AMyBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	replicatedVar = 100.0f;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AMyBox::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);

	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(TestTimer, this, &AMyBox::MulticastRPCExplode, 2.0f, false);
	}
}

// Called every frame
void AMyBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyBox, replicatedVar);
}

void AMyBox::OnRep_ReplicatedVar()
{
	if (HasAuthority()) 
	{
		FVector newLocation = GetActorLocation() + FVector(0.0f, 0.0f, 200.0f);
		SetActorLocation(newLocation);

		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Server: OnRep_ReplicatedVar"));
	}
	else 
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, FString::Printf(TEXT("Client %d: OnRep_ReplicatedVar"), GPlayInEditorID));
	}
}

void AMyBox::DecreaseReplicatedVar()
{
	if(HasAuthority())
	{
		replicatedVar -= 1.0f;
		OnRep_ReplicatedVar();
		if (replicatedVar > 0)
		{
			GetWorld()->GetTimerManager().SetTimer(TestTimer, this, &AMyBox::DecreaseReplicatedVar, 2.0f, false);
		}
	}
}

void AMyBox::MulticastRPCExplode_Implementation()
{
	if(HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Server: MulticastRPCExplode_Implementation"));
		GetWorld()->GetTimerManager().SetTimer(TestTimer, this, &AMyBox::MulticastRPCExplode, 2.0f, false);
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Client: MulticastRPCExplode_Implementation"));
	}

	FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, 200.0f);

	if (!IsRunningDedicatedServer())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, SpawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
	}
}
