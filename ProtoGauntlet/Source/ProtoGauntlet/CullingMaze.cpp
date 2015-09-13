// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtoGauntlet.h"
#include "CullingMaze.h"

void ACullingMaze::SpawnWalls() {
	AMazeWall* CurrentWall;
	PillarLayers = (MazeLengthInTiles - 5) / 8 + 1;
	CurrentDominoDirection = EDirection::D_South;
	float VisibilityOffset = 10.1f; // Keeps the ground from clipping with lowered walls
	for (int y = 0; y < MazeLengthInTiles; y++) {
		Row[y].ColumnWallRef.SetNum(MazeLengthInTiles);
		for (int x = 0; x < MazeLengthInTiles; x++) {
			CurrentWall = Cast<AMazeWall>(GetWorld()->SpawnActor(WallClass));
			if (CurrentWall) {
				CurrentWall->SetActorLocation(GetActorLocation() + FVector((float)(x) * TileSize, (float)(y) * TileSize, -InnerWallHeight + FloorHeight - VisibilityOffset));
				CurrentWall->SetActorScale3D(FVector(TileSize / 100.f, TileSize / 100.f, InnerWallHeight / 100.f));
				Row[y].ColumnWallRef[x] = CurrentWall;
			}
		}

	}

	FTimerHandle PillarTimer;
	GetWorldTimerManager().SetTimer(PillarTimer, this, &ACullingMaze::InitialPillarRaise, 0.1f, false);
	GetWorldTimerManager().SetTimer(DominoHandle, this, &ACullingMaze::DominoWallsFromDirection, 5.f, false);
}

void ACullingMaze::SpawnBorders() {

}

void ACullingMaze::SpawnFloor() {
	AActor* Floor = GetWorld()->SpawnActor(FloorClass);
	Floor->SetActorLocation(GetActorLocation());
	Floor->SetActorScale3D(FVector((float)(MazeLengthInTiles) * TileSize / 100.f, (float)(MazeLengthInTiles) * TileSize / 100.f, FloorHeight / 100.f));

}

void ACullingMaze::InitialPillarRaise() {
	for (int32 y = 3; y < MazeLengthInTiles / 2; y += 4) {
		for (int32 x = 3; x < MazeLengthInTiles / 2; x += 4) {
			StandingPillars.Emplace(Row[y].ColumnWallRef[x]);
			Row[y].ColumnWallRef[x]->Raise();
		}

		for (int32 x = (MazeLengthInTiles + 1) / 2; x < MazeLengthInTiles; x += 4) {
			StandingPillars.Emplace(Row[y].ColumnWallRef[x]);
			Row[y].ColumnWallRef[x]->Raise();
		}

	}
	for (int32 y = (MazeLengthInTiles + 1) / 2; y < MazeLengthInTiles; y += 4) {
		for (int32 x = 3; x < MazeLengthInTiles / 2; x += 4) {
			StandingPillars.Emplace(Row[y].ColumnWallRef[x]);
			Row[y].ColumnWallRef[x]->Raise();
		}

		for (int32 x = (MazeLengthInTiles + 1) / 2; x < MazeLengthInTiles; x += 4) {
			StandingPillars.Emplace(Row[y].ColumnWallRef[x]);
			Row[y].ColumnWallRef[x]->Raise();
		}
	}

}

void ACullingMaze::LowerLayerOfPillars() {
	if (PillarLayers > 1) {
		int32 WallIndex = 0;
		while (WallIndex < StandingPillars.Num()) {
			if ((int32)(StandingPillars[WallIndex]->GetActorLocation().X / TileSize) == MazeLengthInTiles / 2 - 1 - 4 * (PillarLayers - 1)
				|| (int32)(StandingPillars[WallIndex]->GetActorLocation().Y / TileSize) == MazeLengthInTiles / 2 - 1 - 4 * (PillarLayers - 1)
				|| (int32)(StandingPillars[WallIndex]->GetActorLocation().X / TileSize) == MazeLengthInTiles / 2 + 1 + 4 * (PillarLayers - 1)
				|| (int32)(StandingPillars[WallIndex]->GetActorLocation().Y / TileSize) == MazeLengthInTiles / 2 + 1 + 4 * (PillarLayers - 1)) {
				StandingPillars[WallIndex]->Lower();
				StandingPillars.RemoveAt(WallIndex);
			}
			else {
				WallIndex++;
			}
		}
		PillarLayers--;
	}
}

void ACullingMaze::DominoWallsFromDirection() {
	int32 Index = 0;
	
	if (CurrentDominoDirection == EDirection::D_South) {
		if (!StandingPillars.Find(Row[MazeLengthInTiles - DominoEffectRow - 1].ColumnWallRef[MazeLengthInTiles - DominoEffectColumn - 1], Index)) {
			if (DominoEffectRow == 0) { 
				Row[MazeLengthInTiles - DominoEffectRow - 1].ColumnWallRef[MazeLengthInTiles - DominoEffectColumn - 1]->RaiseAndLower();
			} else if (!StandingPillars.Find(Row[MazeLengthInTiles - DominoEffectRow].ColumnWallRef[MazeLengthInTiles - DominoEffectColumn - 1], Index)) {
				Row[MazeLengthInTiles - DominoEffectRow - 1].ColumnWallRef[MazeLengthInTiles - DominoEffectColumn - 1]->RaiseAndLower();
			}
		}

		DominoEffectColumn++;

		if (DominoEffectColumn == MazeLengthInTiles) {
			DominoEffectColumn = 0;
			DominoEffectRow++;
		}

		if (DominoEffectRow == MazeLengthInTiles) {
			DominoEffectRow = 0;
			CurrentDominoDirection = EDirection::D_East;
			GetWorldTimerManager().SetTimer(DominoHandle, this, &ACullingMaze::DominoWallsFromDirection, 5.f, false);
		} else {
			GetWorldTimerManager().SetTimer(DominoHandle, this, &ACullingMaze::DominoWallsFromDirection, 4.f / (6.f * (float)MazeLengthInTiles), false);
		}

		
	} else if (CurrentDominoDirection == EDirection::D_East) {
		if (!StandingPillars.Find(Row[MazeLengthInTiles - DominoEffectRow - 1].ColumnWallRef[MazeLengthInTiles - DominoEffectColumn - 1], Index)) {
			if (DominoEffectColumn == 0) {
				Row[MazeLengthInTiles - DominoEffectRow - 1].ColumnWallRef[MazeLengthInTiles - DominoEffectColumn - 1]->RaiseAndLower();
			}
			else if (!StandingPillars.Find(Row[MazeLengthInTiles - DominoEffectRow - 1].ColumnWallRef[MazeLengthInTiles - DominoEffectColumn], Index)) {
				Row[MazeLengthInTiles - DominoEffectRow - 1].ColumnWallRef[MazeLengthInTiles - DominoEffectColumn - 1]->RaiseAndLower();
			}
		}

		DominoEffectRow++;

		if (DominoEffectRow == MazeLengthInTiles) {
			DominoEffectRow = 0;
			DominoEffectColumn++;
		}

		if (DominoEffectColumn == MazeLengthInTiles) {
			DominoEffectColumn = 0;
			CurrentDominoDirection = EDirection::D_North;
			GetWorldTimerManager().SetTimer(DominoHandle, this, &ACullingMaze::DominoWallsFromDirection, 5.f, false);
		}
		else {
			GetWorldTimerManager().SetTimer(DominoHandle, this, &ACullingMaze::DominoWallsFromDirection, 4.f / (6.f * (float)MazeLengthInTiles), false);
		}

	} else if (CurrentDominoDirection == EDirection::D_North) {
		if (!StandingPillars.Find(Row[DominoEffectRow].ColumnWallRef[DominoEffectColumn], Index)) {
			if (DominoEffectRow == 0) {
				Row[DominoEffectRow].ColumnWallRef[DominoEffectColumn]->RaiseAndLower();
			}
			else if (!StandingPillars.Find(Row[DominoEffectRow - 1].ColumnWallRef[DominoEffectColumn], Index)) {
				Row[DominoEffectRow].ColumnWallRef[DominoEffectColumn]->RaiseAndLower();
			}
		}

		DominoEffectColumn++;

		if (DominoEffectColumn == MazeLengthInTiles) {
			DominoEffectColumn = 0;
			DominoEffectRow++;
		}

		if (DominoEffectRow == MazeLengthInTiles) {
			DominoEffectRow = 0;
			CurrentDominoDirection = EDirection::D_West;
			GetWorldTimerManager().SetTimer(DominoHandle, this, &ACullingMaze::DominoWallsFromDirection, 5.f, false);
		}
		else {
			GetWorldTimerManager().SetTimer(DominoHandle, this, &ACullingMaze::DominoWallsFromDirection, 4.f / (6.f * (float)MazeLengthInTiles), false);
		}

	} else if (CurrentDominoDirection == EDirection::D_West) {
		if (!StandingPillars.Find(Row[DominoEffectRow].ColumnWallRef[DominoEffectColumn], Index)) {
			if (DominoEffectColumn == 0) {
				Row[DominoEffectRow].ColumnWallRef[DominoEffectColumn]->RaiseAndLower();
			}
			else if (!StandingPillars.Find(Row[DominoEffectRow].ColumnWallRef[DominoEffectColumn - 1], Index)) {
				Row[DominoEffectRow].ColumnWallRef[DominoEffectColumn]->RaiseAndLower();
			}
		}

		DominoEffectRow++;

		if (DominoEffectRow == MazeLengthInTiles) {
			DominoEffectRow = 0;
			DominoEffectColumn++;
		}

		if (DominoEffectColumn == MazeLengthInTiles) {
			DominoEffectColumn = 0;
			CurrentDominoDirection = EDirection::D_North;
			GetWorldTimerManager().SetTimer(DominoHandle, this, &ACullingMaze::DominoWallsFromDirection, 5.f, false);
			LowerLayerOfPillars();
		}
		else {
			GetWorldTimerManager().SetTimer(DominoHandle, this, &ACullingMaze::DominoWallsFromDirection, 4.f / (6.f * (float)MazeLengthInTiles), false);
		}
	}

}