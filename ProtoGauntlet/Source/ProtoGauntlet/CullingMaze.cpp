// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtoGauntlet.h"
#include "CullingMaze.h"

ACullingMaze::ACullingMaze() {
	TimeBetweenDirectionChange = 5.f;
	TimeBetweenDominoEffectBetweenToWalls = 4.f / (6.f * (float)MazeLengthInTiles);
}

void ACullingMaze::BeginPlay() {
	Super::BeginPlay();

	PathfindingActive = false;
}

void ACullingMaze::ChangeDominoDirection() {
	if (CurrentDominoFromDirection == EDirection::D_South) {
		DominoEffectRow = 0;
		CurrentDominoFromDirection = EDirection::D_East;
	}
	else if (CurrentDominoFromDirection == EDirection::D_East) {
		DominoEffectColumn = 0;
		CurrentDominoFromDirection = EDirection::D_North;
	}
	else if (CurrentDominoFromDirection == EDirection::D_North) {
		DominoEffectRow = 0;
		CurrentDominoFromDirection = EDirection::D_West;
	}
	else {
		DominoEffectColumn = 0;
		CurrentDominoFromDirection = EDirection::D_South;
		LowerLayerOfPillars();
	}

	GetWorldTimerManager().SetTimer(DominoHandle, this, &ACullingMaze::DominoWallsFromDirection, TimeBetweenDirectionChange, false);
}

void ACullingMaze::DominoWallsFromDirection() {
	if (CurrentDominoFromDirection == EDirection::D_South && CurrentDominoFromDirection == EDirection::D_East) {
		CurrentWallRow = SouthBorder - DominoEffectRow - 1;
		CurrentWallColumn = EastBorder - DominoEffectColumn - 1;

	}
	else if (CurrentDominoFromDirection == EDirection::D_North && CurrentDominoFromDirection == EDirection::D_West) {
		CurrentWallRow = DominoEffectRow;
		CurrentWallColumn = DominoEffectColumn;

	}

	if (!IsStandingPillar(CurrentWallRow, CurrentWallColumn) && !IsBehindStandingPillar(CurrentWallRow, CurrentWallColumn)) {
		Row[CurrentWallRow].ColumnWallRef[CurrentWallColumn]->RaiseAndLower();
	}

	if (CurrentDominoFromDirection == EDirection::D_East || CurrentDominoFromDirection == EDirection::D_West) {
		DominoEffectRow++;

		if (DominoEffectRow == MazeLengthInTiles) {
			SwitchToNextColumnOfWalls();
		}

		if (DominoEffectColumn == MazeLengthInTiles) {
			ChangeDominoDirection();
		}
		else {
			GetWorldTimerManager().SetTimer(DominoHandle, this, &ACullingMaze::DominoWallsFromDirection, TimeBetweenDominoEffectBetweenToWalls, false);
		}
	}else if (CurrentDominoFromDirection == EDirection::D_South || CurrentDominoFromDirection == EDirection::D_North) {
		DominoEffectColumn++;

		if (DominoEffectColumn == MazeLengthInTiles) {
			SwitchToNextRowOfWalls();
		}

		if (DominoEffectRow == MazeLengthInTiles) {
			ChangeDominoDirection();
		}
		else {
			GetWorldTimerManager().SetTimer(DominoHandle, this, &ACullingMaze::DominoWallsFromDirection, TimeBetweenDominoEffectBetweenToWalls, false);
		}
	}

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

bool ACullingMaze::IsBehindStandingPillar(int32 WallRow, int32 WallColumn) {
	bool Result;
	int32 IndexHolderBecauseItIsARequiredParameter;
	int32 SouthNeighborRow = CurrentWallRow + 1;
	int32 EastNeighborColumn = CurrentWallColumn + 1;
	int32 NorthNeighborRow = CurrentWallRow - 1;
	int32 WestNeighborColumn = CurrentWallColumn - 1;

	if (CurrentDominoFromDirection == EDirection::D_South) {
		Result = !((SouthNeighborRow == SouthBorder) || !StandingPillars.Find(Row[SouthNeighborRow].ColumnWallRef[CurrentWallColumn], IndexHolderBecauseItIsARequiredParameter));
	}
	else if (CurrentDominoFromDirection == EDirection::D_East) {
		Result = !((EastNeighborColumn == EastBorder) || !StandingPillars.Find(Row[CurrentWallRow].ColumnWallRef[EastNeighborColumn], IndexHolderBecauseItIsARequiredParameter));
	}
	else if (CurrentDominoFromDirection == EDirection::D_North) {
		Result = !((NorthNeighborRow == NorthBorder) || !StandingPillars.Find(Row[NorthNeighborRow].ColumnWallRef[CurrentWallColumn], IndexHolderBecauseItIsARequiredParameter));
	}
	else if (CurrentDominoFromDirection == EDirection::D_West) {
		Result = !((WestNeighborColumn == WestBorder) || !StandingPillars.Find(Row[CurrentWallRow].ColumnWallRef[WestNeighborColumn], IndexHolderBecauseItIsARequiredParameter));
	}
	return Result;
}

bool ACullingMaze::IsStandingPillar(int32 WallRow, int32 WallColumn){
	int32 IndexHolderBecauseItIsARequiredParameter;
	return StandingPillars.Find(Row[WallRow].ColumnWallRef[WallColumn], IndexHolderBecauseItIsARequiredParameter);
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

void ACullingMaze::SpawnBorders() {

}

void ACullingMaze::SpawnFloor() {
	float FloorHeightScale = FloorHeight * CentimetersToWallSizeScaleRatio;
	float FloorLengthScale = (float)(MazeLengthInTiles)* WallThicknessScale;
	FVector FloorScale = FVector(FloorLengthScale, FloorLengthScale, FloorHeightScale);
	AActor* Floor = GetWorld()->SpawnActor(FloorClass);
	Floor->SetActorLocation(GetActorLocation());
	Floor->SetActorScale3D(FloorScale);

}

void ACullingMaze::SpawnWalls() {
	AMazeWall* CurrentWall;
	PillarLayers = (MazeLengthInTiles - 5) / 8 + 1;
	CurrentDominoFromDirection = EDirection::D_South;
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

void ACullingMaze::SwitchToNextColumnOfWalls() {
	DominoEffectRow = 0;
	DominoEffectColumn++;
}

void ACullingMaze::SwitchToNextRowOfWalls() {
	DominoEffectColumn = 0;
	DominoEffectRow++;
}