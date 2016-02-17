// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtoGauntlet.h"
#include "MegaMaze.h"
#include "MazeSegment.h"


// Sets default values
AMegaMaze::AMegaMaze()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WidthInMazeSegments = 3;
	HeightInMazeSegments = 3;
	TileSize = 400.f;
	MazeLengthInTiles = 41;
	FloorHeight = 100.f;
	InnerWallHeight = 600.f;
	OuterWallHeight = 800.f;
}

// Called when the game starts or when spawned
void AMegaMaze::BeginPlay()
{
	Super::BeginPlay();
	
	AMazeSegment* CurrentSegment;
	FVector SegmentLocation;
	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		for (int32 y = 0; y < HeightInMazeSegments; y++)
		{
			for (int32 x = 0; x < WidthInMazeSegments; x++)
			{
				SegmentLocation = GetActorLocation() + FVector((float)((MazeLengthInTiles + 2) * x) * TileSize, (float)((MazeLengthInTiles + 2) * y) * TileSize, 0.f);
				CurrentSegment = World->SpawnActor<AMazeSegment>(MazeSegmentClass);
				CurrentSegment->SetActorLocation(SegmentLocation, false);
				CurrentSegment->ChangeMazeParameters(MazeLengthInTiles, TileSize, FloorHeight, InnerWallHeight, OuterWallHeight);
				if (WidthInMazeSegments / 2 == x && HeightInMazeSegments / 2 == y)
				{
					//(*CurrentSegment).IsCenterPiece = true;
				}
				//World->SpawnActor<AMyProjectProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
		}
	}
}

void AMegaMaze::CalculateValues()
{
	if (MazeLengthInTiles % 2 == 0) {
		MazeLengthInTiles++;
	}

	if (WidthInMazeSegments % 2 == 0) {
		WidthInMazeSegments++;
	}

	if (HeightInMazeSegments % 2 == 0) {
		HeightInMazeSegments++;
	}

}

// Called every frame
void AMegaMaze::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

#if WITH_EDITOR
void AMegaMaze::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	CalculateValues();

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

