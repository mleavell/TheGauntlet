// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtoGauntlet.h"
#include "MazeWall.h"


// Sets default values
AMazeWall::AMazeWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AMazeWall::BeginPlay()
{
	Super::BeginPlay();
	LowerEnabled = true;
	RaiseEnabled = true;
	
}

// Called every frame
void AMazeWall::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

