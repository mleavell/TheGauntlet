// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtoGauntlet.h"
#include "CullingMaze.h"

ACullingMaze::ACullingMaze() {
	CurrentDominoFromDirection = EDirection::D_South;
	JustAfterHalfway = (MazeLengthInTiles / 2) + 1;
	JustBeforeHalfway = (MazeLengthInTiles / 2) - 1;
	PillarLayers = 5;
	SpaceBetweenPillarLayers = 4;
	TimeBetweenDirectionChange = 5.f;
	TimeBetweenDominoEffectBetweenToWalls = 4.f / (6.f * (float)MazeLengthInTiles);
}

void ACullingMaze::BeginPlay() {
	Super::BeginPlay();

	PathfindingActive = false;

	float MinimalDelay = 0.1f;
	EastOuterMostPillarColumn = JustAfterHalfway + (SpaceBetweenPillarLayers * (PillarLayers - 1));
	NorthernOuterMostPillarRow = JustBeforeHalfway - (SpaceBetweenPillarLayers * (PillarLayers - 1));
	SouthernOuterMostPillarRow = EastOuterMostPillarColumn;
	WestOuterMostPillarColumn = NorthernOuterMostPillarRow;

	FTimerHandle PillarTimer;
	GetWorldTimerManager().SetTimer(PillarTimer, this, &ACullingMaze::InitialPillarRaise, MinimalDelay, false);
	GetWorldTimerManager().SetTimer(DominoHandle, this, &ACullingMaze::DominoWallsFromDirection, TimeBetweenDirectionChange, false);
}

void ACullingMaze::ChangeDominoDirection() {
	LowerLayerOfPillars();
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
	if (CurrentDominoFromDirection == EDirection::D_South || CurrentDominoFromDirection == EDirection::D_East) {
		CurrentWallRow = SouthBorder - DominoEffectRow - 1;
		CurrentWallColumn = EastBorder - DominoEffectColumn - 1;
		
	}
	else if (CurrentDominoFromDirection == EDirection::D_North || CurrentDominoFromDirection == EDirection::D_West) {
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

void ACullingMaze::GetTileIndexAtLocation(FVector Location, int32& TileRow, int32& TileColumn) {
	FVector AdjustedLocation = Location - GetActorLocation();
	AdjustedLocation /= TileSize;
	TileColumn = FGenericPlatformMath::FloorToInt(AdjustedLocation.X);
	TileRow = FGenericPlatformMath::FloorToInt(AdjustedLocation.Y);
}

void ACullingMaze::InitialPillarRaise() {
	RaisePillarsInNorthEastQuadrant();
	RaisePillarsInNorthWestQuadrant();
	RaisePillarsInSouthEastQuadrant();
	RaisePillarsInSouthWestQuadrant();

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
	if (PillarLayers > MinimumPillarLayers) {
		int32 PillarColumn;
		int32 PillarRow;
		int32 CurrentWallIndex = 0;

		while (CurrentWallIndex < StandingPillars.Num()) {
			GetTileIndexAtLocation(StandingPillars[CurrentWallIndex]->GetActorLocation(), PillarRow, PillarColumn);

			if (PillarColumn == WestOuterMostPillarColumn
				|| PillarRow == NorthernOuterMostPillarRow
				|| PillarColumn == EastOuterMostPillarColumn
				|| PillarRow == SouthernOuterMostPillarRow) {
				StandingPillars[CurrentWallIndex]->Lower();
				StandingPillars.RemoveAt(CurrentWallIndex);

			}
			else {
				CurrentWallIndex++;

			}

		}

		PillarLayers--;
		EastOuterMostPillarColumn = JustAfterHalfway + (SpaceBetweenPillarLayers * (PillarLayers - 1));
		NorthernOuterMostPillarRow = JustBeforeHalfway - (SpaceBetweenPillarLayers * (PillarLayers - 1));
		SouthernOuterMostPillarRow = EastOuterMostPillarColumn;
		WestOuterMostPillarColumn = NorthernOuterMostPillarRow;

	}

}

void ACullingMaze::RaisePillarsInNorthEastQuadrant() {
	for (int32 y = JustBeforeHalfway; y > JustBeforeHalfway - (SpaceBetweenPillarLayers * PillarLayers); y -= SpaceBetweenPillarLayers) {
		for (int32 x = JustAfterHalfway; x < JustAfterHalfway + (SpaceBetweenPillarLayers * PillarLayers); x += SpaceBetweenPillarLayers) {
			StandingPillars.Emplace(Row[y].ColumnWallRef[x]);
			Row[y].ColumnWallRef[x]->Raise();

		}

	}

}

void ACullingMaze::RaisePillarsInNorthWestQuadrant() {
	for (int32 y = JustBeforeHalfway; y > JustBeforeHalfway - (SpaceBetweenPillarLayers * PillarLayers); y -= SpaceBetweenPillarLayers) {
		for (int32 x = JustBeforeHalfway; x > JustBeforeHalfway - (SpaceBetweenPillarLayers * PillarLayers); x -= SpaceBetweenPillarLayers) {
			StandingPillars.Emplace(Row[y].ColumnWallRef[x]);
			Row[y].ColumnWallRef[x]->Raise();

		}

	}

}

void ACullingMaze::RaisePillarsInSouthEastQuadrant() {
		for (int32 y = JustAfterHalfway; y < JustAfterHalfway + (SpaceBetweenPillarLayers * PillarLayers); y += SpaceBetweenPillarLayers) {
		for (int32 x = JustAfterHalfway; x < JustAfterHalfway + (SpaceBetweenPillarLayers * PillarLayers); x += SpaceBetweenPillarLayers) {
			StandingPillars.Emplace(Row[y].ColumnWallRef[x]);
			Row[y].ColumnWallRef[x]->Raise();

		}

	}

}

void ACullingMaze::RaisePillarsInSouthWestQuadrant() {
	for (int32 y = JustAfterHalfway; y < JustAfterHalfway + (SpaceBetweenPillarLayers * PillarLayers); y += SpaceBetweenPillarLayers) {
		for (int32 x = JustBeforeHalfway; x > JustBeforeHalfway - (SpaceBetweenPillarLayers * PillarLayers); x -= SpaceBetweenPillarLayers) {
			StandingPillars.Emplace(Row[y].ColumnWallRef[x]);
			Row[y].ColumnWallRef[x]->Raise();

		}

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
	FVector LocationOffset;

	for (int y = 0; y < MazeLengthInTiles; y++) {
		Row[y].ColumnWallRef.SetNum(MazeLengthInTiles);

		for (int x = 0; x < MazeLengthInTiles; x++) {
			CurrentWall = Cast<AMazeWall>(GetWorld()->SpawnActor(WallClass));

			if (CurrentWall) {
				LocationOffset = FVector((float)(x)* TileSize, (float)(y)* TileSize, -InnerWallHeight + FloorHeight - FloorClippingPreventionOffset);
				CurrentWall->SetActorLocation(GetActorLocation() + LocationOffset);
				CurrentWall->SetActorScale3D(WallScale);
				Row[y].ColumnWallRef[x] = CurrentWall;

			}

		}

	}

}

void ACullingMaze::SwitchToNextColumnOfWalls() {
	DominoEffectRow = 0;
	DominoEffectColumn++;
}

void ACullingMaze::SwitchToNextRowOfWalls() {
	DominoEffectColumn = 0;
	DominoEffectRow++;
}