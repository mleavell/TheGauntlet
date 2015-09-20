// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtoGauntlet.h"
#include "AscensionMaze.h"

void AAscensionMaze::CreateMazeLayout() {
	FMazeRowData WallRow;

	WallRow.Column.Init(ETileDesignation::TD_Path, MazeLengthInTiles);
	for (int32 index = 0; index < MazeLengthInTiles; index++) {
		Row.Add(WallRow);
	}

}

void AAscensionMaze::BeginPlay() {
	Super::BeginPlay();

	FTimerHandle LayerTimer;
	GetWorldTimerManager().SetTimer(LayerTimer, this, &AAscensionMaze::Descend, 0.1f, false);

}

void AAscensionMaze::SpawnBorders() {

}

void AAscensionMaze::SpawnFloor() {

}

void AAscensionMaze::SpawnWalls() {
	NumberOfLayers = (MazeLengthInTiles - 9) / 8 + 1;
	NumberOfAscensions = NumberOfLayers * 2 - 2;
	LevelOfAscension = NumberOfLayers;
	EastWalls.SetNum(0);
	NorthWalls.SetNum(0);
	SouthWalls.SetNum(0);
	WestWalls.SetNum(0);

	for (int32 Layer = 1; Layer < NumberOfLayers; Layer++) {
		EastWalls.Emplace(Cast<AMazeWall>(GetWorld()->SpawnActor(WallClass)));
		EastWalls[Layer - 1]->SetActorLocation(GetActorLocation() + FVector((float)(MazeLengthInTiles - 4 * (Layer)) * TileSize, (4 * (Layer - 1)) * TileSize, 0.f));
		EastWalls[Layer - 1]->SetActorScale3D(FVector( 4.f * TileSize / 100.f, (float)(MazeLengthInTiles - 8 * (Layer - 1))* TileSize / 100.f, InnerWallHeight / 100.f));

		NorthWalls.Emplace(Cast<AMazeWall>(GetWorld()->SpawnActor(WallClass)));
		NorthWalls[Layer - 1]->SetActorLocation(GetActorLocation() + FVector((4 * (Layer - 1)) * TileSize, (4 * (Layer - 1)) * TileSize, 0.f));
		NorthWalls[Layer - 1]->SetActorScale3D(FVector((float)(MazeLengthInTiles - 8 * (Layer - 1))* TileSize / 100.f, 4.f * TileSize / 100.f, InnerWallHeight / 100.f));

		SouthWalls.Emplace(Cast<AMazeWall>(GetWorld()->SpawnActor(WallClass)));
		SouthWalls[Layer - 1]->SetActorLocation(GetActorLocation() + FVector((4 * (Layer - 1)) * TileSize, (float)(MazeLengthInTiles - 4 * (Layer)) * TileSize, 0.f));
		SouthWalls[Layer - 1]->SetActorScale3D(FVector((float)(MazeLengthInTiles - 8 * (Layer - 1))* TileSize / 100.f, 4.f * TileSize / 100.f, InnerWallHeight / 100.f));
		
		WestWalls.Emplace(Cast<AMazeWall>(GetWorld()->SpawnActor(WallClass)));
		WestWalls[Layer - 1]->SetActorLocation(GetActorLocation() + FVector((float)(4 * (Layer - 1)) * TileSize, (4 * (Layer - 1)) * TileSize, 0.f));
		WestWalls[Layer - 1]->SetActorScale3D(FVector(4.f * TileSize / 100.f, (float)(MazeLengthInTiles - 8 * (Layer - 1))* TileSize / 100.f, InnerWallHeight / 100.f));
		
	}

	Centerpiece = Cast<AMazeWall>(GetWorld()->SpawnActor(WallClass));
	Centerpiece->SetActorLocation(GetActorLocation() + FVector((4 * (NumberOfLayers - 1)) * TileSize, (4 * (NumberOfLayers - 1)) * TileSize, 0.f));
	Centerpiece->SetActorScale3D(FVector((float)(MazeLengthInTiles - 8 * (NumberOfLayers - 1))* TileSize / 100.f, (float)(MazeLengthInTiles - 8 * (NumberOfLayers - 1))* TileSize / 100.f, InnerWallHeight / 100.f));

}

void AAscensionMaze::Descend() {
	if (LevelOfAscension > 1) {
		for (int32 LayerToLower = LevelOfAscension - 1; LayerToLower > 0; LayerToLower--) {
			EastWalls[NumberOfLayers - LayerToLower - 1]->Lower();
			NorthWalls[NumberOfLayers - LayerToLower - 1]->Lower();
			SouthWalls[NumberOfLayers - LayerToLower - 1]->Lower();
			WestWalls[NumberOfLayers - LayerToLower - 1]->Lower();
			EastWalls[NumberOfLayers - LayerToLower - 1]->LowerEnabled = true;
			NorthWalls[NumberOfLayers - LayerToLower - 1]->LowerEnabled = true;
			SouthWalls[NumberOfLayers - LayerToLower - 1]->LowerEnabled = true;
			WestWalls[NumberOfLayers - LayerToLower - 1]->LowerEnabled = true;
		}
		Centerpiece->Lower();
		Centerpiece->LowerEnabled = true;
		LevelOfAscension--;

		FTimerHandle LayerTimer;
		GetWorldTimerManager().SetTimer(LayerTimer, this, &AAscensionMaze::Descend, 2.1f, false);

	} else if (LevelOfAscension > 0) {
		Centerpiece->Lower();
		Centerpiece->LowerEnabled = true;
		LevelOfAscension--;
		FTimerHandle LayerTimer;
		GetWorldTimerManager().SetTimer(LayerTimer, this, &AAscensionMaze::Ascend, 2.1f, false);
	}

}

void AAscensionMaze::Ascend() {
	if (LevelOfAscension >= NumberOfLayers - 1 && LevelOfAscension != NumberOfAscensions) {
		if (LevelOfAscension % 4 != 3) {
			for (int32 LayerToLower = LevelOfAscension % 4 + 1; LayerToLower > 0; LayerToLower--) {
				EastWalls[NumberOfLayers - LayerToLower - 1]->Raise();
				NorthWalls[NumberOfLayers - LayerToLower - 1]->Raise();
				SouthWalls[NumberOfLayers - LayerToLower - 1]->Raise();
				WestWalls[NumberOfLayers - LayerToLower - 1]->Raise();
				EastWalls[NumberOfLayers - LayerToLower - 1]->RaiseEnabled = true;
				NorthWalls[NumberOfLayers - LayerToLower - 1]->RaiseEnabled = true;
				SouthWalls[NumberOfLayers - LayerToLower - 1]->RaiseEnabled = true;
				WestWalls[NumberOfLayers - LayerToLower - 1]->RaiseEnabled = true;
			}
		}
		Centerpiece->Raise();
		Centerpiece->RaiseEnabled = true;
		LevelOfAscension++;

	} else if (LevelOfAscension < NumberOfLayers - 1 && LevelOfAscension != 0) {
		for (int32 LayerToLower = LevelOfAscension; LayerToLower > 0; LayerToLower--) {
			EastWalls[NumberOfLayers - LayerToLower - 1]->Raise();
			NorthWalls[NumberOfLayers - LayerToLower - 1]->Raise();
			SouthWalls[NumberOfLayers - LayerToLower - 1]->Raise();
			WestWalls[NumberOfLayers - LayerToLower - 1]->Raise();
			EastWalls[NumberOfLayers - LayerToLower - 1]->RaiseEnabled = true;
			NorthWalls[NumberOfLayers - LayerToLower - 1]->RaiseEnabled = true;
			SouthWalls[NumberOfLayers - LayerToLower - 1]->RaiseEnabled = true;
			WestWalls[NumberOfLayers - LayerToLower - 1]->RaiseEnabled = true;
		}
		Centerpiece->Raise();
		Centerpiece->RaiseEnabled = true;
		LevelOfAscension++;

	} else if (LevelOfAscension == 0) {
		Centerpiece->Raise();
		Centerpiece->RaiseEnabled = true;
		LevelOfAscension++;

	} else {

	}

	FTimerHandle LayerTimer;
	GetWorldTimerManager().SetTimer(LayerTimer, this, &AAscensionMaze::Ascend, 2.1f, false);
}