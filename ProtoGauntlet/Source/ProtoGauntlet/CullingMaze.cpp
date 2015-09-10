// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtoGauntlet.h"
#include "CullingMaze.h"

void ACullingMaze::SpawnWalls() {
	AMazeWall* CurrentWall;
	for (int y = 0; y < MazeLengthInTiles; y++) {
		Row[y].ColumnWallRef.SetNum(MazeLengthInTiles);
		for (int x = 0; x < MazeLengthInTiles; x++) {
			CurrentWall = Cast<AMazeWall>(GetWorld()->SpawnActor(WallClass));
			if (CurrentWall) {
				CurrentWall->SetActorLocation(GetActorLocation() + FVector((float)(x + 1) * TileSize, (float)(y + 1) * TileSize, -InnerWallHeight));
				CurrentWall->SetActorScale3D(FVector(TileSize / 100.f, TileSize / 100.f, InnerWallHeight / 100.f));
				Row[y].ColumnWallRef[x] = CurrentWall;
			}
		}

	}
}

void ACullingMaze::SpawnFloor() {

}

void ACullingMaze::SpawnBorders() {

}