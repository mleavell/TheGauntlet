// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtoGauntlet.h"
#include "WallManager.h"
#include "MazeSegment.h"

bool AMazeSegment::AboveGroundLevelWalls(AActor* InputActor) {
	return InputActor->GetActorLocation().Z > (InnerWallHeight + FloorHeight);
}

// Sets default values
AMazeSegment::AMazeSegment() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CentimetersToWallSizeScaleRatio = 1 / 100.f;
	FloorClippingPreventionOffset = 2.f;
	HasMaze = true;
	NavMeshReady = false;
	TileSize = 400.f;
	MazeLengthInTiles = 41;
	FloorHeight = 100.f;
	InnerWallHeight = 600.f;
	OuterWallHeight = 800.f;
	NorthBorder = -1;
	WestBorder = -1;
	SouthBorder = MazeLengthInTiles;
	EastBorder = MazeLengthInTiles;

}

// Called when the game starts or when spawned
void AMazeSegment::BeginPlay() {
	Super::BeginPlay();
	InitializeWallScaleRatios();
	SpawnFloor();
	SpawnBorders();
	CreateMazeLayout();

	if (HasMaze) {
		SpawnWalls();
	}

	PathfindingActive = true;
	
	

}

void AMazeSegment::CalculateValues() {
	HalfTileSize = TileSize / 2.f;
	if (MazeLengthInTiles % 2 == 0) {
		MazeLengthInTiles++;
	}
}

void AMazeSegment::ChangeMazeParameters(int32 MazeLengthInTiles, float TileSize, float FloorHeight, float InnerWallHeight, float OuterWallHeight) {
	this->TileSize = TileSize;
	this->HalfTileSize = TileSize / 2.f;
	this->FloorHeight = FloorHeight;
	this->InnerWallHeight = InnerWallHeight;
	this->OuterWallHeight = OuterWallHeight;
	this->MazeLengthInTiles = MazeLengthInTiles;

	if (MazeLengthInTiles % 2 == 1) {
		this->MazeLengthInTiles++;
	}

	this->SouthBorder = MazeLengthInTiles;
	this->EastBorder = MazeLengthInTiles;

}

FIntPair AMazeSegment::ChooseRandomValidNeighbor(TArray<FIntPair>& ValidNeighbors) {
	return ValidNeighbors[FMath::RandRange(0, ValidNeighbors.Num() - 1)];
}

void AMazeSegment::ConnectCurrentTileToPath(TArray<FMazeRowData>& Row,TArray<FIntPair>& Path, FIntPair CurrentTile, bool CreatingMazeLayout) {
	if (CreatingMazeLayout) {
		if (Path.Num() > 0) {
			if (CurrentTile.y + 2 == Path.Last().y) {
				Row[CurrentTile.y + 1].Column[CurrentTile.x] = ETileDesignation::TD_Path;

			}
			else if (CurrentTile.y - 2 == Path.Last().y) {
				Row[CurrentTile.y - 1].Column[CurrentTile.x] = ETileDesignation::TD_Path;

			}
			else if (CurrentTile.x + 2 == Path.Last().x) {
				Row[CurrentTile.y].Column[CurrentTile.x + 1] = ETileDesignation::TD_Path;

			}
			else {
				Row[CurrentTile.y].Column[CurrentTile.x - 1] = ETileDesignation::TD_Path;

			}
		}
	}
	Row[CurrentTile.y].Column[CurrentTile.x] = CreatingMazeLayout ? ETileDesignation::TD_Path : ETileDesignation::TD_Visited;
	Path.Push(CurrentTile);
}

void AMazeSegment::CreateMazeLayout() {
	FormatMazeDataArrayForMazeGeneration();

	if (HasMaze) {
		TArray<FIntPair> TileStack;
		TArray<FIntPair> ValidNeighbors;
		FIntPair CurrentTileInPath = RandomTileThatStartsAsCell();
		ConnectCurrentTileToPath(Row, TileStack, CurrentTileInPath, true);

		while (TileStack.Num() > 0) {
			ValidNeighbors = GetValidNeighborsForContinuedPathCreation(Row, CurrentTileInPath, true);

			if (ValidNeighbors.Num() != 0) {
				CurrentTileInPath = ChooseRandomValidNeighbor(ValidNeighbors);
				ConnectCurrentTileToPath(Row, TileStack, CurrentTileInPath, true);

			} else {
				TileStack.Pop();

				if (TileStack.Num() != 0) {
					CurrentTileInPath = TileStack.Last();

				}

			}

		}

	}

}

void AMazeSegment::CreateRandomPathFromStartPoint(FIntPair StartPoint, TArray<FIntPair>& Result, int32 DesiredPathLength) {
	if (IsValidTileLocation(StartPoint.y, StartPoint.x) && !TileIsWall(StartPoint.y, StartPoint.x)) {
		TArray<FMazeRowData> CopyOfRow;
		FIntPair StackHead;
		TArray<FIntPair> ValidNeighbors;
		
		CopyOfRow.Append(Row);
		StackHead = StartPoint;
		ConnectCurrentTileToPath(CopyOfRow, Result, StackHead);

		while (Result.Num() < DesiredPathLength && Result.Num() != 0) {
			ValidNeighbors = GetValidNeighborsForContinuedPathCreation(CopyOfRow, StackHead);

			if (ValidNeighbors.Num() != 0) {
				StackHead = ChooseRandomValidNeighbor(ValidNeighbors);
				ConnectCurrentTileToPath(CopyOfRow, Result, StackHead);
			}
			else {
				Result.Pop();
				if (Result.Num() != 0) {
					StackHead = Result.Last();
				}
			}
		}
	}
};

void AMazeSegment::CreateRandomPathFromStartPointBP(int32 StartPointX, int32 StartPointY, TArray<FVector>& Result, int32 PathLength) {
	TArray<FIntPair> FIntPairPath;

	CreateRandomPathFromStartPoint(FIntPair(StartPointX, StartPointY), FIntPairPath, PathLength);
	IntPairArraytoVectorArray(FIntPairPath, Result);
}

void AMazeSegment::ExtractCorners(TArray<FIntPair> InputArray, TArray<FIntPair>& Result) {
	Result.Add(InputArray[0]);
	for (int i = 1; i < InputArray.Num() - 1; i++) {
		if (InputArray[i].x == InputArray[i - 1].x && InputArray[i].x != InputArray[i + 1].x) {
			Result.Add(InputArray[i]);
		}
		else if (InputArray[i].y == InputArray[i - 1].y && InputArray[i].y != InputArray[i + 1].y) {
			Result.Add(InputArray[i]);
		}
	}
	Result.Add(InputArray.Last());
}

void AMazeSegment::ExtractCornersBP(TArray<FVector> InputArray, TArray<FVector>& Result) {
	TArray<FIntPair> FIntPairArray;
	TArray<FIntPair> ExtractedFIntPairArray;

	VectorArraytoIntPairArray(InputArray, FIntPairArray);
	ExtractCorners(FIntPairArray, ExtractedFIntPairArray);
	IntPairArraytoVectorArray(ExtractedFIntPairArray, Result);

}

void AMazeSegment::FindPathBetweenPoints(FIntPair StartPoint, FIntPair EndPoint, TArray<FIntPair>& Path, EDirection StartDirection) {
	if (IsValidTileLocation(StartPoint.y, StartPoint.x) && !TileIsWall(StartPoint.y, StartPoint.x) && !TileIsWall(EndPoint.y, EndPoint.x)) {
		TArray<FMazeRowData> CopyOfRow;
		FIntPair StackHead;
		TArray<FIntPair> ValidNeighbors;

		CopyOfRow.Append(Row);
		StackHead = StartPoint;
		ConnectCurrentTileToPath(CopyOfRow, Path, StackHead);
		MarkSurroundingTilesAsVisitedExcept(StartDirection, StartPoint, CopyOfRow);

		while (Path.Num() != 0 && Path.Last() != EndPoint) {
			ValidNeighbors = GetValidNeighborsForContinuedPathCreation(CopyOfRow, StackHead);

			if (ValidNeighbors.Num() != 0) {
				StackHead = ChooseRandomValidNeighbor(ValidNeighbors);
				ConnectCurrentTileToPath(CopyOfRow, Path, StackHead);
			}
			else {
				Path.Pop();
				if (Path.Num() != 0) {
					StackHead = Path.Last();
				}
			}
		}
	}
};

void AMazeSegment::FindPathBetweenPointsBP(int32 StartPointX, int32 StartPointY, int32 EndPointX, int32 EndPointY, TArray<FVector>& Path, EDirection StartDirection) {
	TArray<FIntPair> FIntPairPath;

	FindPathBetweenPoints(FIntPair(StartPointX, StartPointY), FIntPair(EndPointX, EndPointY), FIntPairPath, StartDirection);
	IntPairArraytoVectorArray(FIntPairPath, Path);
}

void AMazeSegment::FormatMazeDataArrayForMazeGeneration() {
	if (Row.Num() == 0) {
		if (HasMaze) {
			FMazeRowData CellRow;
			FMazeRowData WallRow;

			WallRow.Column.Init(ETileDesignation::TD_Wall, MazeLengthInTiles);
			for (int32 index = 0; index < MazeLengthInTiles; index++) {
				if (index % 2 == 1) {
					CellRow.Column.Add(ETileDesignation::TD_Wall);
				}
				else {
					CellRow.Column.Add(ETileDesignation::TD_Cell);
				}

			}

			for (int32 index = 0; index < MazeLengthInTiles; index++) {
				if (index % 2 == 1) {
					Row.Add(WallRow);
				}
				else {
					Row.Add(CellRow);
				}
			}

		}
		else {
			FMazeRowData PathRow;
			PathRow.Column.Init(ETileDesignation::TD_Path, MazeLengthInTiles);
			for (int32 index = 0; index < MazeLengthInTiles; index++) {
				Row.Add(PathRow);

			}
		}
	}

}

void AMazeSegment::GetAllTilesInSection(FIntPair StartPoint, TArray<FIntPair>& Result, EDirection StartDirection) {
	if (IsValidTileLocation(StartPoint.y, StartPoint.x) && !TileIsWall(StartPoint.y, StartPoint.x)) {
		TArray<FMazeRowData> CopyOfRow;
		TArray<FIntPair> PathStack;
		FIntPair StackHead;
		TArray<FIntPair> ValidNeighbors;

		CopyOfRow.Append(Row);
		StackHead = StartPoint;
		ConnectCurrentTileToPath(CopyOfRow, Result, StackHead);
		ConnectCurrentTileToPath(CopyOfRow, PathStack, StackHead);
		MarkSurroundingTilesAsVisitedExcept(StartDirection, StartPoint, CopyOfRow);

		while (PathStack.Num() != 0) {
			ValidNeighbors = GetValidNeighborsForContinuedPathCreation(CopyOfRow, StackHead);

			if (ValidNeighbors.Num() != 0) {
				StackHead = ChooseRandomValidNeighbor(ValidNeighbors);
				ConnectCurrentTileToPath(CopyOfRow, PathStack, StackHead);
				ConnectCurrentTileToPath(CopyOfRow, Result, StackHead);
			}
			else {
				PathStack.Pop();
				if (PathStack.Num() != 0) {
					StackHead = PathStack.Last();
				}
			}
		}
	}
};

void AMazeSegment::GetDirectionsFromVectorArray(TArray<FVector> PathArray, TArray<uint8>& DirectionArray) {
	for (int32 i = 1; i < PathArray.Num(); i++) {
		if (FMath::Abs(PathArray[i].X - PathArray[i - 1].X) > 0.001f) {
			if (PathArray[i].X - PathArray[i - 1].X > 0.f) {
				DirectionArray.Add(StaticCast<uint8>(EDirection::D_East));

			}

			else {
				DirectionArray.Add(StaticCast<uint8>(EDirection::D_West));

			}

		}
		else if (FMath::Abs(PathArray[i].Y - PathArray[i - 1].Y) > 0.001f) {
			if (PathArray[i].Y - PathArray[i - 1].Y > 0.f) {
				DirectionArray.Add(StaticCast<uint8>(EDirection::D_South));

			}
			else {
				DirectionArray.Add(StaticCast<uint8>(EDirection::D_North));

			}
		}
	}
}

void AMazeSegment::GetLocationOfTile(FVector& Location, int32 TileRow, int32 TileColumn) {
	Location = FVector((TileColumn + 1) * TileSize, (TileRow + 1) * TileSize, FloorHeight);
}

FMazeInfo AMazeSegment::GetMazeInfo() {
	return FMazeInfo(MazeLengthInTiles, FloorHeight, TileSize, WallScale);
}

int32 AMazeSegment::GetMazeLengthInTiles() {
	return MazeLengthInTiles;
}

bool AMazeSegment::GetPathfindingActive() {
	return PathfindingActive;
}

ETileDesignation AMazeSegment::GetTileDesignationAt(int32 TileRow, int32 TileColumn) {
	if (TileRow > NorthBorder && TileRow < SouthBorder && TileColumn > WestBorder && TileColumn < EastBorder) {
		return Row[TileRow].Column[TileColumn];
	}

	return ETileDesignation::TD_OutOfBounds;
}

void AMazeSegment::GetTileIndexAtLocation(FVector Location, int32& TileRow, int32& TileColumn) {
	int32 AdjustmentForBorder = 1;
	FVector AdjustedLocation = Location - GetActorLocation();
	AdjustedLocation /= TileSize;
	TileColumn = FGenericPlatformMath::FloorToInt(AdjustedLocation.X) - AdjustmentForBorder;
	TileRow = FGenericPlatformMath::FloorToInt(AdjustedLocation.Y) - AdjustmentForBorder;
}

TArray<FIntPair>& AMazeSegment::GetValidNeighborsForContinuedPathCreation(TArray<FMazeRowData>& Row, FIntPair CurrentTileInPath, bool CreatingMazeLayout) {
	int32 NeighborOffsetSize;
	ETileDesignation CorrectNeighborDesignation;
	static TArray<FIntPair> ValidNeighbors;
	ValidNeighbors.Empty();

	if (CreatingMazeLayout) {
		CorrectNeighborDesignation = ETileDesignation::TD_Cell;
		NeighborOffsetSize = 2;

	} else {
		CorrectNeighborDesignation = ETileDesignation::TD_Path;
		NeighborOffsetSize = 1;

	}

	int32 NorthNeighborY = CurrentTileInPath.y - NeighborOffsetSize;
	int32 WestNeighborX = CurrentTileInPath.x - NeighborOffsetSize;
	int32 SouthNeighborY = CurrentTileInPath.y + NeighborOffsetSize;
	int32 EastNeighborX = CurrentTileInPath.x + NeighborOffsetSize;

	if (NorthNeighborY > NorthBorder && Row[NorthNeighborY].Column[CurrentTileInPath.x] == CorrectNeighborDesignation) {
		ValidNeighbors.Add(FIntPair(CurrentTileInPath.x, NorthNeighborY));
	}

	if (SouthNeighborY < SouthBorder && Row[SouthNeighborY].Column[CurrentTileInPath.x] == CorrectNeighborDesignation) {
		ValidNeighbors.Add(FIntPair(CurrentTileInPath.x, SouthNeighborY));
	}

	if (WestNeighborX > WestBorder && Row[CurrentTileInPath.y].Column[WestNeighborX] == CorrectNeighborDesignation) {
		ValidNeighbors.Add(FIntPair(WestNeighborX, CurrentTileInPath.y));
	}

	if (EastNeighborX < EastBorder && Row[CurrentTileInPath.y].Column[EastNeighborX] == CorrectNeighborDesignation) {
		ValidNeighbors.Add(FIntPair(EastNeighborX, CurrentTileInPath.y));
	}

	return ValidNeighbors;

}

void AMazeSegment::InitializeWallScaleRatios() {
	WallThicknessScale = TileSize * CentimetersToWallSizeScaleRatio;
	WallLengthScale = (float)((MazeLengthInTiles + 2) / 2) * WallThicknessScale;
	OuterWallHeightScale = OuterWallHeight * CentimetersToWallSizeScaleRatio;
	InnerWallHeightScale = InnerWallHeight * CentimetersToWallSizeScaleRatio;
	WallScale = FVector(WallThicknessScale, WallThicknessScale, InnerWallHeightScale);
}

void AMazeSegment::IntPairArraytoVectorArray(TArray<FIntPair> InputArray, TArray<FVector>& Result) {
	FVector TileLocation;
	for (FIntPair CurrentTile : InputArray) {
		GetLocationOfTile(TileLocation, CurrentTile.y, CurrentTile.x);
		Result.Add(TileLocation + FVector(HalfTileSize, HalfTileSize, 0.f));
	}
}

bool AMazeSegment::IsCorner(int32 TileRow, int32 TileColumn) {
	bool Result = false;
	if (IsValidTileLocation(TileRow, TileColumn)) {
		if (IsValidTileLocation(TileRow - 1, TileColumn) &&
			Row[TileRow - 1].Column[TileColumn] == ETileDesignation::TD_Path ||
			(IsValidTileLocation(TileRow + 1, TileColumn) &&
			Row[TileRow + 1].Column[TileColumn] == ETileDesignation::TD_Path)) {
			if ((IsValidTileLocation(TileRow, TileColumn - 1) &&
				Row[TileRow].Column[TileColumn - 1] == ETileDesignation::TD_Path) ||
				(IsValidTileLocation(TileRow, TileColumn + 1) &&
				Row[TileRow].Column[TileColumn + 1] == ETileDesignation::TD_Path)) {
				Result = true;

			}

		}

	}
	return Result;

}

bool AMazeSegment::IsIntersection(int32 TileRow, int32 TileColumn) {
	bool Result = false;
	if (IsValidTileLocation(TileRow, TileColumn)) {
		int32 ValidNeighbors = 0;

		if (IsValidTileLocation(TileRow - 1, TileColumn) &&
			Row[TileRow - 1].Column[TileColumn] == ETileDesignation::TD_Path) {
			ValidNeighbors++;
		}

		if (IsValidTileLocation(TileRow + 1, TileColumn) &&
			Row[TileRow + 1].Column[TileColumn] == ETileDesignation::TD_Path) {
			ValidNeighbors++;
		}

		if (IsValidTileLocation(TileRow, TileColumn - 1) &&
			Row[TileRow].Column[TileColumn - 1] == ETileDesignation::TD_Path) {
			ValidNeighbors++;
		}

		if (IsValidTileLocation(TileRow, TileColumn + 1) &&
			Row[TileRow].Column[TileColumn + 1] == ETileDesignation::TD_Path) {
			ValidNeighbors++;
		}

		if (ValidNeighbors >= 3) {
			Result = true;
		}
	}
	return Result;

}

bool AMazeSegment::IsValidTileLocation(int32 TileRow, int32 TileColumn) {
	return TileColumn >= 0 && TileRow >= 0 && TileColumn < MazeLengthInTiles && TileRow < MazeLengthInTiles;
}

void AMazeSegment::MarkSurroundingTilesAsVisitedExcept(EDirection UnvisitedDirection, FIntPair CurrentTile, TArray<FMazeRowData>& CopyOfRow) {
	if (UnvisitedDirection != EDirection::D_North && CurrentTile.y > NorthBorder) {
		CopyOfRow[CurrentTile.y - 1].Column[CurrentTile.x] = ETileDesignation::TD_Visited;
	}

	if (UnvisitedDirection != EDirection::D_East && CurrentTile.x < EastBorder) {
		CopyOfRow[CurrentTile.y].Column[CurrentTile.x + 1] = ETileDesignation::TD_Visited;
	}

	if (UnvisitedDirection != EDirection::D_South && CurrentTile.y < SouthBorder) {
		CopyOfRow[CurrentTile.y + 1].Column[CurrentTile.x] = ETileDesignation::TD_Visited;
	}

	if (UnvisitedDirection != EDirection::D_West && CurrentTile.x > WestBorder) {
		CopyOfRow[CurrentTile.y].Column[CurrentTile.x - 1] = ETileDesignation::TD_Visited;
	}
	
}

void AMazeSegment::NextIntersection(FIntPair StartPoint, FIntPair& Intersection, EDirection StartDirection, int32 MaxDistance) {
	if (IsValidTileLocation(StartPoint.y, StartPoint.x) &&
		Row[StartPoint.y].Column[StartPoint.x] != ETileDesignation::TD_Wall) {
		if (!IsIntersection(StartPoint.y, StartPoint.x)) {
			TArray<FMazeRowData> CopyOfRow;
			CopyOfRow.Append(Row);
			CopyOfRow[StartPoint.y].Column[StartPoint.x] = ETileDesignation::TD_Visited;
			TArray<FIntPair> PathStack;
			PathStack.Add(StartPoint);

			if (StartDirection == EDirection::D_North && StartPoint.y - 1 > 0 && CopyOfRow[StartPoint.y - 1].Column[StartPoint.x] == ETileDesignation::TD_Path) {
				if (IsValidTileLocation(StartPoint.y, StartPoint.x - 1)) {
					CopyOfRow[StartPoint.y].Column[StartPoint.x - 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y, StartPoint.x + 1)) {
					CopyOfRow[StartPoint.y].Column[StartPoint.x + 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y + 1, StartPoint.x)) {
					CopyOfRow[StartPoint.y + 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}
			}
			else if (StartDirection == EDirection::D_East && StartPoint.x + 1 < MazeLengthInTiles && CopyOfRow[StartPoint.y].Column[StartPoint.x + 1] == ETileDesignation::TD_Path) {
				if (IsValidTileLocation(StartPoint.y, StartPoint.x - 1)) {
					CopyOfRow[StartPoint.y].Column[StartPoint.x - 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y - 1, StartPoint.x)) {
					CopyOfRow[StartPoint.y - 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y + 1, StartPoint.x)) {
					CopyOfRow[StartPoint.y + 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}
			}
			else if (StartDirection == EDirection::D_South && StartPoint.y + 1< MazeLengthInTiles && CopyOfRow[StartPoint.y + 1].Column[StartPoint.x] == ETileDesignation::TD_Path) {
				if (IsValidTileLocation(StartPoint.y, StartPoint.x - 1)) {
					CopyOfRow[StartPoint.y].Column[StartPoint.x - 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y, StartPoint.x + 1)) {
					CopyOfRow[StartPoint.y].Column[StartPoint.x + 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y - 1, StartPoint.x)) {
					CopyOfRow[StartPoint.y - 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}
			}
			else if (StartDirection == EDirection::D_West && StartPoint.x - 1 > 0 && CopyOfRow[StartPoint.y].Column[StartPoint.x - 1] == ETileDesignation::TD_Path) {
				if (IsValidTileLocation(StartPoint.y, StartPoint.x + 1)) {
					CopyOfRow[StartPoint.y].Column[StartPoint.x + 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y - 1, StartPoint.x)) {
					CopyOfRow[StartPoint.y - 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y + 1, StartPoint.x)) {
					CopyOfRow[StartPoint.y + 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}
			}
			else {
				PathStack.Pop();

			}

			TArray<FIntPair> ValidNeighbors;
			FIntPair StackHead = StartPoint;
			while (PathStack.Num() != 0 &&
				(PathStack.Num() - 2 > MaxDistance || !IsIntersection(StackHead.y, StackHead.x))) {
				ValidNeighbors.SetNum(0);

				// Upper Neighbor
				if (StackHead.y - 1 >= 0 && CopyOfRow[StackHead.y - 1].Column[StackHead.x] == ETileDesignation::TD_Path) {
					ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y - 1));
				}

				// Lower Neighbor
				if (StackHead.y + 1 < MazeLengthInTiles && CopyOfRow[StackHead.y + 1].Column[StackHead.x] == ETileDesignation::TD_Path) {
					ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y + 1));
				}

				// Left Neighbor
				if (StackHead.x - 1 >= 0 && CopyOfRow[StackHead.y].Column[StackHead.x - 1] == ETileDesignation::TD_Path) {
					ValidNeighbors.Add(FIntPair(StackHead.x - 1, StackHead.y));
				}

				// Right Neighbor
				if (StackHead.x + 1 < MazeLengthInTiles && CopyOfRow[StackHead.y].Column[StackHead.x + 1] == ETileDesignation::TD_Path) {
					ValidNeighbors.Add(FIntPair(StackHead.x + 1, StackHead.y));
				}

				if (ValidNeighbors.Num() != 0) {
					// Choose random valid neighbor
					StackHead = ValidNeighbors[FMath::RandRange(0, ValidNeighbors.Num() - 1)];
					CopyOfRow[StackHead.y].Column[StackHead.x] = ETileDesignation::TD_Visited;
					PathStack.Push(StackHead);
				}
				else {
					PathStack.Pop();
					if (PathStack.Num() != 0) {
						StackHead = PathStack.Last();
					}
				}
			}

			if (PathStack.Num() == 0) {
				Intersection = FIntPair(-1, -1);
			}
			else {
				Intersection = PathStack.Last();
			}

		}
		else {
			Intersection = StartPoint;

		}
	}
}

void AMazeSegment::NextIntersectionBP(int32 StartPointX, int32 StartPointY, int32& IntersectionX, int32& IntersectionY, EDirection StartDirection, int32 MaxDistance) {
	FIntPair Intersection;

	NextIntersection(FIntPair(StartPointX, StartPointY), Intersection, StartDirection, MaxDistance);
	IntersectionX = Intersection.x;
	IntersectionY = Intersection.y;
}

bool AMazeSegment::PathHasIntersectionBP(TArray<FVector> Path, int32& IntersectionX, int32& IntersectionY) {
	TArray<FIntPair> FIntPairPath;
	bool IntersectionFound = false;
	IntersectionX = -1;
	IntersectionY = -1;

	VectorArraytoIntPairArray(Path, FIntPairPath);
	for (FIntPair CurrentTile : FIntPairPath) {
		if (!IntersectionFound &&  IsIntersection(CurrentTile.y, CurrentTile.x)) {
			IntersectionFound = true;
			IntersectionX = CurrentTile.x;
			IntersectionY = CurrentTile.y;
		}
	}
	return IntersectionFound;
}

#if WITH_EDITOR
void AMazeSegment::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	CalculateValues();

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AMazeSegment::PostInitProperties()
{
	Super::PostInitProperties();

	CalculateValues();
}

FIntPair AMazeSegment::RandomTileThatStartsAsCell() {
	return FIntPair(FMath::RandRange(0, MazeLengthInTiles / 2) * 2, FMath::RandRange(0, MazeLengthInTiles / 2) * 2);

}

void AMazeSegment::ResetMazeLayout() {
	for (int y = 0; y < MazeLengthInTiles; y++) {
		for (int x = 0; x < MazeLengthInTiles; x++) {
			if (y % 2 == 0 && x % 2 == 0) {
				Row[y].Column[x] = ETileDesignation::TD_Cell;

			}else {
				Row[y].Column[x] = ETileDesignation::TD_Wall;

			}

		}

	}

}

void AMazeSegment::SpawnBorders() {
	float MazeLengthPlusBorderWall = (float)(MazeLengthInTiles + 1) * TileSize;
	float HalfMazeLengthPlusBorderWall = (float)((MazeLengthInTiles + 2) / 2 + 1) * TileSize;
	FVector BorderWallScale = FVector(WallThicknessScale, WallLengthScale, OuterWallHeightScale);

	FVector LocationOffset = FVector(0.f, 0.f, FloorHeight);
	AActor* WestBorderWallHalf = GetWorld()->SpawnActor(BorderClass);
	WestBorderWallHalf->SetActorLocation(GetActorLocation() + LocationOffset);
	WestBorderWallHalf->SetActorScale3D(BorderWallScale);

	LocationOffset = FVector(0.f, HalfMazeLengthPlusBorderWall, FloorHeight);
	WestBorderWallHalf = GetWorld()->SpawnActor(BorderClass);
	WestBorderWallHalf->SetActorLocation(GetActorLocation() + LocationOffset);
	WestBorderWallHalf->SetActorScale3D(BorderWallScale);

	LocationOffset = FVector(MazeLengthPlusBorderWall, 0.f, FloorHeight);
	AActor* EastBorderWallHalf = GetWorld()->SpawnActor(BorderClass);
	EastBorderWallHalf->SetActorLocation(GetActorLocation() + LocationOffset);
	EastBorderWallHalf->SetActorScale3D(BorderWallScale);

	LocationOffset = FVector(MazeLengthPlusBorderWall, HalfMazeLengthPlusBorderWall, FloorHeight);
	EastBorderWallHalf = GetWorld()->SpawnActor(BorderClass);
	EastBorderWallHalf->SetActorLocation(GetActorLocation() + LocationOffset);
	EastBorderWallHalf->SetActorScale3D(BorderWallScale);

	WallLengthScale -= WallThicknessScale;
	BorderWallScale = FVector(WallLengthScale, WallThicknessScale, OuterWallHeightScale);

	LocationOffset = FVector(TileSize, 0.f, FloorHeight);
	AActor* NorthBorderWallHalf = GetWorld()->SpawnActor(BorderClass);
	NorthBorderWallHalf->SetActorLocation(GetActorLocation() + LocationOffset);
	NorthBorderWallHalf->SetActorScale3D(BorderWallScale);

	LocationOffset = FVector(HalfMazeLengthPlusBorderWall, 0.f, FloorHeight);
	NorthBorderWallHalf = GetWorld()->SpawnActor(BorderClass);
	NorthBorderWallHalf->SetActorLocation(GetActorLocation() + LocationOffset);
	NorthBorderWallHalf->SetActorScale3D(BorderWallScale);

	LocationOffset = FVector(TileSize, MazeLengthPlusBorderWall, FloorHeight);
	AActor* SouthBorderWallHalf = GetWorld()->SpawnActor(BorderClass);
	SouthBorderWallHalf->SetActorLocation(GetActorLocation() + LocationOffset);
	SouthBorderWallHalf->SetActorScale3D(BorderWallScale);

	LocationOffset = FVector(HalfMazeLengthPlusBorderWall, MazeLengthPlusBorderWall, FloorHeight);
	SouthBorderWallHalf = GetWorld()->SpawnActor(BorderClass);
	SouthBorderWallHalf->SetActorLocation(GetActorLocation() + LocationOffset);
	SouthBorderWallHalf->SetActorScale3D(BorderWallScale);

}

void AMazeSegment::SpawnFloor() {
	float FloorHeightScale = FloorHeight * CentimetersToWallSizeScaleRatio;
	float FloorLengthScale = (float)(MazeLengthInTiles + 2) * WallThicknessScale;
	FVector FloorScale = FVector(FloorLengthScale, FloorLengthScale, FloorHeightScale);
	AActor* Floor = GetWorld()->SpawnActor(FloorClass);
	Floor->SetActorLocation(GetActorLocation());
	Floor->SetActorScale3D(FloorScale);

}

void AMazeSegment::SpawnWalls() {
	WallManagerRef = Cast<AWallManager>(GetWorld()->SpawnActor(WallManagementClass));
	FVector NorthWestCorner = FVector(TileSize, TileSize, FloorHeight);
	WallManagerRef->SetActorLocation(NorthWestCorner);
	WallManagerRef->SpawnWalls(this);
}

// Called every frame
void AMazeSegment::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

bool AMazeSegment::TileIsWall(int32 TileRow, int32 TileColumn) {
	return Row[TileRow].Column[TileColumn] == ETileDesignation::TD_Wall;
}

void AMazeSegment::VectorArraytoIntPairArray(TArray<FVector> InputArray, TArray<FIntPair>& Result) {
	FIntPair TileIndex;
	for (FVector CurrentLocation : InputArray) {
		GetTileIndexAtLocation(CurrentLocation, TileIndex.y, TileIndex.x);
		Result.Add(TileIndex);
	}
}

// uint8 test = (uint8)ETileDesignation::TD_Cell;
//const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ETileDesignation"), true);
//FString testString = EnumPtr->GetEnumName(test);
////FText testText = FText::FromString(testString);

//TArray<ETileDesignation> testArray;
//for (int index = 0; index < MazeLengthInTiles; index++) {
//	if (index % 2 == 1) {
//		testArray.Add(ETileDesignation::TD_Wall);
//	}
//	else {
//		testArray.Add(ETileDesignation::TD_Cell);
//	}
//}

//	Here so I can just copy paste a print to screen: GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString(TEXT("Here Inactive Walls")));
//testString = FString::FromInt(testArray.Num());
/*for (auto& currentTile : WallRow.Column) {
testString = EnumPtr->GetEnumName((uint8)currentTile);
GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, testString);
}*/
/*for (auto& ColumnData : Row) {
CopyOfRow.Emplace(ColumnData);

}*/