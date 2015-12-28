// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtoGauntlet.h"
#include "ShapeshifterMaze.h"

void AShapeshifterMaze::SpawnWalls() {
	AMazeWall* CurrentWall;
	float VisibilityOffset = 10.1f; // Keeps the ground from clipping with lowered walls
	for (int y = 0; y < MazeLengthInTiles; y++) {
		Row[y].ColumnWallRef.SetNum(MazeLengthInTiles);
		for (int x = 0; x < MazeLengthInTiles; x++) {
			if (y % 2 == 1 || x % 2 == 1) {
				CurrentWall = Cast<AMazeWall>(GetWorld()->SpawnActor(WallClass));
				if (CurrentWall) {
					CurrentWall->SetActorLocation(GetActorLocation() + FVector((float)(x + 1) * TileSize, (float)(y + 1) * TileSize, FloorHeight - VisibilityOffset ));
					CurrentWall->SetActorScale3D(FVector(TileSize / 100.f, TileSize / 100.f, InnerWallHeight / 100.f));
					Row[y].ColumnWallRef[x] = CurrentWall;
				}
			}
		}
	}
	FTimerHandle InitalWallLower;
	GetWorldTimerManager().SetTimer(InitalWallLower, this, &AShapeshifterMaze::LowerInactiveWalls, 0.1f, false);
}

void AShapeshifterMaze::BeginPlay() {
	Super::BeginPlay();

	PathfindingActive = false;
	FTimerHandle ShapeshiftTimer;
	GetWorldTimerManager().SetTimer(ShapeshiftTimer, this, &AShapeshifterMaze::Shapeshift, ShapeshiftDelay, false);
}

AShapeshifterMaze::AShapeshifterMaze() {
	ShapeshiftDelay = 5.f;
	PostDepowerDelay = 5.f;
}

void AShapeshifterMaze::RaiseAllWalls() {
	AMazeWall* CurrentWall;
	float VisibilityOffset = 10.1f; // Keeps the ground from clipping with lowered walls
	for (int y = 0; y < MazeLengthInTiles; y++) {
		for (int x = 0; x < MazeLengthInTiles; x++) {
			if (y % 2 == 1 || x % 2 == 1) {
				CurrentWall = Row[y].ColumnWallRef[x];
				if (CurrentWall) {
					if (CurrentWall->LowerEnabled == false) {
						CurrentWall->Raise();
					}
				}
			}
		}

	}
}

void AShapeshifterMaze::ShuffleMazeLayout() {
	ResetMazeLayout();
	CreateMazeLayout();
}

void AShapeshifterMaze::LowerInactiveWalls() {
	AMazeWall* CurrentWall;
	for (int y = 0; y < MazeLengthInTiles; y++) {
		for (int x = 0; x < MazeLengthInTiles; x++) {
			CurrentWall = Row[y].ColumnWallRef[x];
			if (CurrentWall) {
				if (Row[y].Column[x] == ETileDesignation::TD_Path) {
					CurrentWall->Lower();
				}
			}
		}

	}
}
//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString(TEXT("Here Inactive Walls")));