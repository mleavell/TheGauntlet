// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtoGauntlet.h"
#include "ExpandingArena.h"

AExpandingArena::AExpandingArena() {
	DesiredLayerOfWallsLowered = 3;
}

void AExpandingArena::BeginPlay() {
	Super::BeginPlay();

	PathfindingActive = true;
	FTimerHandle LayerTimer;
	if (DesiredLayerOfWallsLowered > CurrentLayerOfWallsLowered) {
		GetWorldTimerManager().SetTimer(LayerTimer, this, &AExpandingArena::LowerLayerOfWalls, 0.1f, false);

	} else if (DesiredLayerOfWallsLowered < CurrentLayerOfWallsLowered) {
		GetWorldTimerManager().SetTimer(LayerTimer, this, &AExpandingArena::RaiseLayerOfWalls, 0.1f, false);

	}

}

void AExpandingArena::CreateMazeLayout() {
	FMazeRowData WallRow;

	WallRow.Column.Init(ETileDesignation::TD_Wall, MazeLengthInTiles);
	for (int32 index = 0; index < MazeLengthInTiles; index++) {
		Row.Add(WallRow);
	}

}

void AExpandingArena::SpawnWalls() {
	CurrentLayerOfWallsLowered = 0;
	AMazeWall* CurrentWall;
	float VisibilityOffset = 10.1f; // Keeps the ground from clipping with lowered walls
	RowWalls.SetNum(0);
	ColumnWalls.SetNum(0);

	for (int x = 0; x < MazeLengthInTiles; x++) {
		CurrentWall = Cast<AMazeWall>(GetWorld()->SpawnActor(WallClass));
		if (CurrentWall) {
			CurrentWall->SetActorLocation(GetActorLocation() + FVector(TileSize, (float)(x + 1) * TileSize, FloorHeight - VisibilityOffset));
			CurrentWall->SetActorScale3D(FVector((float)(MazeLengthInTiles) * TileSize / 100.f, TileSize / 100.f, InnerWallHeight / 100.f));
			RowWalls.Emplace(CurrentWall);
		}

		CurrentWall = Cast<AMazeWall>(GetWorld()->SpawnActor(WallClass));
		if (CurrentWall) {
			CurrentWall->SetActorLocation(GetActorLocation() + FVector((float)(x + 1) * TileSize, TileSize, FloorHeight - VisibilityOffset));
			CurrentWall->SetActorScale3D(FVector(TileSize / 100.f, (float)(MazeLengthInTiles) * TileSize / 100.f, InnerWallHeight / 100.f));
			ColumnWalls.Emplace(CurrentWall);
		}
	}

}

void AExpandingArena::LowerLayerOfWalls() {
	RowWalls[MazeLengthInTiles / 2 - CurrentLayerOfWallsLowered]->Lower();
	ColumnWalls[MazeLengthInTiles / 2 - CurrentLayerOfWallsLowered]->Lower();
	RowWalls[MazeLengthInTiles / 2 + CurrentLayerOfWallsLowered]->Lower();
	ColumnWalls[MazeLengthInTiles / 2 + CurrentLayerOfWallsLowered]->Lower();
	CurrentLayerOfWallsLowered++;
	FTimerHandle LayerTimer;

	if (DesiredLayerOfWallsLowered > CurrentLayerOfWallsLowered) {
		GetWorldTimerManager().SetTimer(LayerTimer, this, &AExpandingArena::LowerLayerOfWalls, 2.1f, false);

	}
	
}

void AExpandingArena::RaiseLayerOfWalls() {
	RowWalls[MazeLengthInTiles / 2 - CurrentLayerOfWallsLowered + 1]->Raise();
	ColumnWalls[MazeLengthInTiles / 2 - CurrentLayerOfWallsLowered + 1]->Raise();
	RowWalls[MazeLengthInTiles / 2 + CurrentLayerOfWallsLowered - 1]->Raise();
	ColumnWalls[MazeLengthInTiles / 2 + CurrentLayerOfWallsLowered - 1]->Raise();
	CurrentLayerOfWallsLowered--;
	FTimerHandle LayerTimer;

	if (DesiredLayerOfWallsLowered < CurrentLayerOfWallsLowered) {
		GetWorldTimerManager().SetTimer(LayerTimer, this, &AExpandingArena::RaiseLayerOfWalls, 2.1f, false);

	}
}

void AExpandingArena::ChangeDesiredLayer(int32 ChosenLayer) {
	DesiredLayerOfWallsLowered = ChosenLayer;
	FTimerHandle LayerTimer;

	if (DesiredLayerOfWallsLowered > CurrentLayerOfWallsLowered) {
		GetWorldTimerManager().SetTimer(LayerTimer, this, &AExpandingArena::LowerLayerOfWalls, 2.1f, false);

	} else if (DesiredLayerOfWallsLowered < CurrentLayerOfWallsLowered) {
		GetWorldTimerManager().SetTimer(LayerTimer, this, &AExpandingArena::RaiseLayerOfWalls, 2.1f, false);

	}
}

int32 AExpandingArena::GetCurrentLayerOfWallsLowered() {
	return CurrentLayerOfWallsLowered;
}