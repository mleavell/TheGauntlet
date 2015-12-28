// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtoGauntlet.h"
#include "MazeSegment.h"


// Sets default values
AMazeSegment::AMazeSegment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	IsCenterPiece = false;
	NavMeshReady = false;
	TileSize = 400.f;
	MazeLengthInTiles = 41;
	FloorHeight = 100.f;
	InnerWallHeight = 600.f;
	OuterWallHeight = 800.f;

}

// Called when the game starts or when spawned
void AMazeSegment::BeginPlay()
{
	Super::BeginPlay();
	SpawnFloor();
	SpawnBorders();
	CreateMazeLayout();

	if (!IsCenterPiece) {
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

void AMazeSegment::ChangeMazeParameters(int32 MazeLengthInTiles, float TileSize, float FloorHeight, float InnerWallHeight, float OuterWallHeight)
{
	this->TileSize = TileSize;
	this->HalfTileSize = TileSize / 2.f;
	this->MazeLengthInTiles = MazeLengthInTiles;
	this->FloorHeight = FloorHeight;
	this->InnerWallHeight = InnerWallHeight;
	this->OuterWallHeight = OuterWallHeight;
}

FIntPair AMazeSegment::ChooseRandomValidNeighbor(TArray<FIntPair>& ValidNeighbors) {
	return ValidNeighbors[FMath::RandRange(0, ValidNeighbors.Num() - 1)];
}

void AMazeSegment::ConnectCurrentTileToPath(TArray<FIntPair>& Path, FIntPair CurrentTile) {
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

	Row[CurrentTile.y].Column[CurrentTile.x] = ETileDesignation::TD_Path;
	Path.Push(CurrentTile);
}

void AMazeSegment::CreateMazeLayout() {
	if (!IsCenterPiece) {
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

		TArray<FIntPair> TileStack;
		TArray<FIntPair> ValidNeighbors;
		FIntPair StackHead;

		TileStack.Push(StackHead);
		Row[0].Column[0] = ETileDesignation::TD_Path;

		while (TileStack.Num()) {
			ValidNeighbors.SetNum(0);

			// Upper Neighbor
			if (StackHead.y - 2 >= 0 && Row[StackHead.y - 2].Column[StackHead.x] == ETileDesignation::TD_Cell) {
				ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y - 2));
			}

			// Lower Neighbor
			if (StackHead.y + 2 < MazeLengthInTiles && Row[StackHead.y + 2].Column[StackHead.x] == ETileDesignation::TD_Cell) {
				ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y + 2));
			}

			// Left Neighbor
			if (StackHead.x - 2 >= 0 && Row[StackHead.y].Column[StackHead.x - 2] == ETileDesignation::TD_Cell) {
				ValidNeighbors.Add(FIntPair(StackHead.x - 2, StackHead.y));
			}

			// Right Neighbor
			if (StackHead.x + 2 < MazeLengthInTiles && Row[StackHead.y].Column[StackHead.x + 2] == ETileDesignation::TD_Cell) {
				ValidNeighbors.Add(FIntPair(StackHead.x + 2, StackHead.y));
			}

			if (ValidNeighbors.Num() != 0) {
				// Choose random valid neighbor
				StackHead = ValidNeighbors[FMath::RandRange(0, ValidNeighbors.Num() - 1)];

				if (StackHead.y + 2 == TileStack.Last().y) {
					Row[StackHead.y + 1].Column[StackHead.x] = ETileDesignation::TD_Path;

				}
				else if (StackHead.y - 2 == TileStack.Last().y) {
					Row[StackHead.y - 1].Column[StackHead.x] = ETileDesignation::TD_Path;

				}
				else if (StackHead.x + 2 == TileStack.Last().x) {
					Row[StackHead.y].Column[StackHead.x + 1] = ETileDesignation::TD_Path;

				}
				else {
					Row[StackHead.y].Column[StackHead.x - 1] = ETileDesignation::TD_Path;

				}

				Row[StackHead.y].Column[StackHead.x] = ETileDesignation::TD_Path;
				TileStack.Push(StackHead);
			}
			else {
				TileStack.Pop();
				if (TileStack.Num() != 0) {
					StackHead = TileStack.Last();
				}
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

void AMazeSegment::CreateRandomPathFromStartPoint(FIntPair StartPoint, TArray<FIntPair> & Result, int32 PathLength) {
	if (IsValidTileLocation(StartPoint.y, StartPoint.x) && Row[StartPoint.y].Column[StartPoint.x] != ETileDesignation::TD_Wall) {
		TArray<FMazeRowData> CopyRow;
		/*for (auto& ColumnData : Row) {
		CopyRow.Emplace(ColumnData);

		}*/
		CopyRow.Append(Row);
		CopyRow[StartPoint.y].Column[StartPoint.x] = ETileDesignation::TD_Visited;
		Result.Add(StartPoint);

		TArray<FIntPair> ValidNeighbors;
		FIntPair StackHead = StartPoint;
		while (Result.Num() < PathLength && Result.Num() != 0) {
			ValidNeighbors.SetNum(0);

			// Upper Neighbor
			if (StackHead.y - 1 >= 0 && CopyRow[StackHead.y - 1].Column[StackHead.x] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y - 1));
			}

			// Lower Neighbor
			if (StackHead.y + 1 < MazeLengthInTiles && CopyRow[StackHead.y + 1].Column[StackHead.x] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y + 1));
			}

			// Left Neighbor
			if (StackHead.x - 1 >= 0 && CopyRow[StackHead.y].Column[StackHead.x - 1] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x - 1, StackHead.y));
			}

			// Right Neighbor
			if (StackHead.x + 1 < MazeLengthInTiles && CopyRow[StackHead.y].Column[StackHead.x + 1] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x + 1, StackHead.y));
			}

			if (ValidNeighbors.Num() != 0) {
				// Choose random valid neighbor
				StackHead = ValidNeighbors[FMath::RandRange(0, ValidNeighbors.Num() - 1)];
				CopyRow[StackHead.y].Column[StackHead.x] = ETileDesignation::TD_Visited;
				Result.Push(StackHead);
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

void AMazeSegment::CreateRandomPathFromStartPointBP(int32 StartPointX, int32 StartPointY, TArray<FVector> & Result, int32 PathLength) {
	TArray<FIntPair> FIntPairPath;
	CreateRandomPathFromStartPoint(FIntPair(StartPointX, StartPointY), FIntPairPath, PathLength);
	IntPairArraytoVectorArray(FIntPairPath, Result);
}

void AMazeSegment::ExtractCorners(TArray<FIntPair> InputArray, TArray<FIntPair> & Result) {
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

void AMazeSegment::ExtractCornersBP(TArray<FVector> InputArray, TArray<FVector> & Result) {
	TArray<FIntPair> FIntPairArray;
	TArray<FIntPair> ExtractedFIntPairArray;
	VectorArraytoIntPairArray(InputArray, FIntPairArray);
	ExtractCorners(FIntPairArray, ExtractedFIntPairArray);
	IntPairArraytoVectorArray(ExtractedFIntPairArray, Result);

}

void AMazeSegment::FindPathBetweenPoints(FIntPair StartPoint, FIntPair EndPoint, TArray<FIntPair> & Path, EDirection StartDirection) {
	if (IsValidTileLocation(StartPoint.y, StartPoint.x)){//&& Row[StartPoint.y].Column[StartPoint.x] != ETileDesignation::TD_Wall && Row[EndPoint.y].Column[EndPoint.x] != ETileDesignation::TD_Wall) {
		TArray<FMazeRowData> CopyRow;
		/*for (auto& ColumnData : Row) {
		CopyRow.Emplace(ColumnData);

		}*/
		CopyRow.Append(Row);
		CopyRow[StartPoint.y].Column[StartPoint.x] = ETileDesignation::TD_Visited;
		Path.Add(StartPoint);

		if (StartDirection != EDirection::D_None) {
			if (StartDirection == EDirection::D_North && StartPoint.y > 0 && CopyRow[StartPoint.y - 1].Column[StartPoint.x] == ETileDesignation::TD_Path) {
				if (IsValidTileLocation(StartPoint.y, StartPoint.x - 1)) {
					CopyRow[StartPoint.y].Column[StartPoint.x - 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y, StartPoint.x + 1)) {
					CopyRow[StartPoint.y].Column[StartPoint.x + 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y + 1, StartPoint.x)) {
					CopyRow[StartPoint.y + 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}

			}
			else if (StartDirection == EDirection::D_East && StartPoint.x < MazeLengthInTiles && CopyRow[StartPoint.y].Column[StartPoint.x + 1] == ETileDesignation::TD_Path) {
				if (IsValidTileLocation(StartPoint.y, StartPoint.x - 1)) {
					CopyRow[StartPoint.y].Column[StartPoint.x - 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y - 1, StartPoint.x)) {
					CopyRow[StartPoint.y - 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y + 1, StartPoint.x)) {
					CopyRow[StartPoint.y + 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}

			}
			else if (StartDirection == EDirection::D_South && StartPoint.y < MazeLengthInTiles && CopyRow[StartPoint.y + 1].Column[StartPoint.x] == ETileDesignation::TD_Path) {
				if (IsValidTileLocation(StartPoint.y, StartPoint.x - 1)) {
					CopyRow[StartPoint.y].Column[StartPoint.x - 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y, StartPoint.x + 1)) {
					CopyRow[StartPoint.y].Column[StartPoint.x + 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y - 1, StartPoint.x)) {
					CopyRow[StartPoint.y - 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}

			}
			else if (StartDirection == EDirection::D_West && StartPoint.x > 0 && CopyRow[StartPoint.y].Column[StartPoint.x - 1] == ETileDesignation::TD_Path) {
				if (IsValidTileLocation(StartPoint.y, StartPoint.x + 1)) {
					CopyRow[StartPoint.y].Column[StartPoint.x + 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y - 1, StartPoint.x)) {
					CopyRow[StartPoint.y - 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y + 1, StartPoint.x)) {
					CopyRow[StartPoint.y + 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}

			}
			else {
				Path.Pop();
			}
		}

		TArray<FIntPair> ValidNeighbors;
		FIntPair StackHead = StartPoint;
		while (Path.Num() != 0 && Path.Last() != EndPoint) {
			ValidNeighbors.SetNum(0);

			// Upper Neighbor
			if (StackHead.y - 1 >= 0 && CopyRow[StackHead.y - 1].Column[StackHead.x] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y - 1));
			}

			// Lower Neighbor
			if (StackHead.y + 1 < MazeLengthInTiles && CopyRow[StackHead.y + 1].Column[StackHead.x] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y + 1));
			}

			// Left Neighbor
			if (StackHead.x - 1 >= 0 && CopyRow[StackHead.y].Column[StackHead.x - 1] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x - 1, StackHead.y));
			}

			// Right Neighbor
			if (StackHead.x + 1 < MazeLengthInTiles && CopyRow[StackHead.y].Column[StackHead.x + 1] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x + 1, StackHead.y));
			}

			if (ValidNeighbors.Num() != 0) {
				// Choose random valid neighbor
				StackHead = ValidNeighbors[FMath::RandRange(0, ValidNeighbors.Num() - 1)];
				CopyRow[StackHead.y].Column[StackHead.x] = ETileDesignation::TD_Visited;
				Path.Push(StackHead);
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

void AMazeSegment::FindPathBetweenPointsBP(int32 StartPointX, int32 StartPointY, int32 EndPointX, int32 EndPointY, TArray<FVector> & Path, EDirection StartDirection) {
	TArray<FIntPair> FIntPairPath;
	FindPathBetweenPoints(FIntPair(StartPointX, StartPointY), FIntPair(EndPointX, EndPointY), FIntPairPath, StartDirection);
	IntPairArraytoVectorArray(FIntPairPath, Path);
}

void AMazeSegment::GetAllTilesInSection(FIntPair StartPoint, TArray<FIntPair> & Result, EDirection StartDirection) {
	if (IsValidTileLocation(StartPoint.y, StartPoint.x) &&
		Row[StartPoint.y].Column[StartPoint.x] != ETileDesignation::TD_Wall) {
		TArray<FMazeRowData> CopyRow;
		/*for (auto& ColumnData : Row) {
		CopyRow.Emplace(ColumnData);

		}*/
		CopyRow.Append(Row);
		CopyRow[StartPoint.y].Column[StartPoint.x] = ETileDesignation::TD_Visited;
		TArray<FIntPair> PathStack;
		PathStack.Add(StartPoint);
		Result.Add(StartPoint);

		if (StartDirection == EDirection::D_North && StartPoint.y > 0 && CopyRow[StartPoint.y - 1].Column[StartPoint.x] == ETileDesignation::TD_Path) {
			if (IsValidTileLocation(StartPoint.y, StartPoint.x - 1)) {
				CopyRow[StartPoint.y].Column[StartPoint.x - 1] = ETileDesignation::TD_Visited;
			}
			if (IsValidTileLocation(StartPoint.y, StartPoint.x + 1)) {
				CopyRow[StartPoint.y].Column[StartPoint.x + 1] = ETileDesignation::TD_Visited;
			}
			if (IsValidTileLocation(StartPoint.y + 1, StartPoint.x)) {
				CopyRow[StartPoint.y + 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
			}
		}
		else if (StartDirection == EDirection::D_East && StartPoint.x < MazeLengthInTiles && CopyRow[StartPoint.y].Column[StartPoint.x + 1] == ETileDesignation::TD_Path) {
			if (IsValidTileLocation(StartPoint.y, StartPoint.x - 1)) {
				CopyRow[StartPoint.y].Column[StartPoint.x - 1] = ETileDesignation::TD_Visited;
			}
			if (IsValidTileLocation(StartPoint.y - 1, StartPoint.x)) {
				CopyRow[StartPoint.y - 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
			}
			if (IsValidTileLocation(StartPoint.y + 1, StartPoint.x)) {
				CopyRow[StartPoint.y + 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
			}
		}
		else if (StartDirection == EDirection::D_South && StartPoint.y < MazeLengthInTiles && CopyRow[StartPoint.y + 1].Column[StartPoint.x] == ETileDesignation::TD_Path) {
			if (IsValidTileLocation(StartPoint.y, StartPoint.x - 1)) {
				CopyRow[StartPoint.y].Column[StartPoint.x - 1] = ETileDesignation::TD_Visited;
			}
			if (IsValidTileLocation(StartPoint.y, StartPoint.x + 1)) {
				CopyRow[StartPoint.y].Column[StartPoint.x + 1] = ETileDesignation::TD_Visited;
			}
			if (IsValidTileLocation(StartPoint.y - 1, StartPoint.x)) {
				CopyRow[StartPoint.y - 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
			}
		}
		else if (StartDirection == EDirection::D_West && StartPoint.x > 0 && CopyRow[StartPoint.y].Column[StartPoint.x - 1] == ETileDesignation::TD_Path) {
			if (IsValidTileLocation(StartPoint.y, StartPoint.x + 1)) {
				CopyRow[StartPoint.y].Column[StartPoint.x + 1] = ETileDesignation::TD_Visited;
			}
			if (IsValidTileLocation(StartPoint.y - 1, StartPoint.x)) {
				CopyRow[StartPoint.y - 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
			}
			if (IsValidTileLocation(StartPoint.y + 1, StartPoint.x)) {
				CopyRow[StartPoint.y + 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
			}
		}
		else {
			Result.Pop();
			PathStack.Pop();
		}

		TArray<FIntPair> ValidNeighbors;
		FIntPair StackHead = StartPoint;
		while (PathStack.Num() != 0) {
			ValidNeighbors.SetNum(0);

			// Upper Neighbor
			if (StackHead.y - 1 >= 0 && CopyRow[StackHead.y - 1].Column[StackHead.x] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y - 1));
			}

			// Lower Neighbor
			if (StackHead.y + 1 < MazeLengthInTiles && CopyRow[StackHead.y + 1].Column[StackHead.x] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y + 1));
			}

			// Left Neighbor
			if (StackHead.x - 1 >= 0 && CopyRow[StackHead.y].Column[StackHead.x - 1] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x - 1, StackHead.y));
			}

			// Right Neighbor
			if (StackHead.x + 1 < MazeLengthInTiles && CopyRow[StackHead.y].Column[StackHead.x + 1] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x + 1, StackHead.y));
			}

			if (ValidNeighbors.Num() != 0) {
				// Choose random valid neighbor
				StackHead = ValidNeighbors[FMath::RandRange(0, ValidNeighbors.Num() - 1)];
				CopyRow[StackHead.y].Column[StackHead.x] = ETileDesignation::TD_Visited;
				PathStack.Push(StackHead);
				Result.Push(StackHead);
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

void AMazeSegment::GetDirectionsFromVectorArray(TArray<FVector> PathArray, TArray<uint8> & DirectionArray) {
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

void AMazeSegment::GetLocationOfTile(FVector & Location, int32 TileRow, int32 TileColumn) {
	Location = FVector((TileColumn + 1) * TileSize, (TileRow + 1) * TileSize, FloorHeight);
}

bool AMazeSegment::GetPathfindingActive() {
	return PathfindingActive;
}

ETileDesignation AMazeSegment::GetTileDesignationAt(int32 TileRow, int32 TileColumn) {
	if (TileRow >= 0 && TileRow < MazeLengthInTiles && TileColumn >= 0 && TileColumn < MazeLengthInTiles) {
		return Row[TileRow].Column[TileColumn];
	}

	return ETileDesignation::TD_OutOfBounds;
}

void AMazeSegment::GetTileIndexAtLocation(FVector Location, int32 & TileRow, int32 & TileColumn) {
	FVector AdjustedLocation = Location - GetActorLocation();
	AdjustedLocation /= TileSize;
	TileColumn = FGenericPlatformMath::FloorToInt(AdjustedLocation.X) - 1;
	TileRow = FGenericPlatformMath::FloorToInt(AdjustedLocation.Y) - 1;
}

TArray<FIntPair>& AMazeSegment::GetValidNeighborsForContinuedPathCreation(FIntPair CurrentTileInPath) {
	static TArray<FIntPair> ValidNeighbors;
	ValidNeighbors.Empty();

	// Upper Neighbor
	if (CurrentTileInPath.y - 2 >= 0 && Row[CurrentTileInPath.y - 2].Column[CurrentTileInPath.x] == ETileDesignation::TD_Cell) {
		ValidNeighbors.Add(FIntPair(CurrentTileInPath.x, CurrentTileInPath.y - 2));
	}

	// Lower Neighbor
	if (CurrentTileInPath.y + 2 < MazeLengthInTiles && Row[CurrentTileInPath.y + 2].Column[CurrentTileInPath.x] == ETileDesignation::TD_Cell) {
		ValidNeighbors.Add(FIntPair(CurrentTileInPath.x, CurrentTileInPath.y + 2));
	}

	// Left Neighbor
	if (CurrentTileInPath.x - 2 >= 0 && Row[CurrentTileInPath.y].Column[CurrentTileInPath.x - 2] == ETileDesignation::TD_Cell) {
		ValidNeighbors.Add(FIntPair(CurrentTileInPath.x - 2, CurrentTileInPath.y));
	}

	// Right Neighbor
	if (CurrentTileInPath.x + 2 < MazeLengthInTiles && Row[CurrentTileInPath.y].Column[CurrentTileInPath.x + 2] == ETileDesignation::TD_Cell) {
		ValidNeighbors.Add(FIntPair(CurrentTileInPath.x + 2, CurrentTileInPath.y));
	}

	return ValidNeighbors;

}

void AMazeSegment::IntPairArraytoVectorArray(TArray<FIntPair> InputArray, TArray<FVector> & Result) {
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

void AMazeSegment::NextIntersection(FIntPair StartPoint, FIntPair & Intersection, EDirection StartDirection, int32 MaxDistance) {
	if (IsValidTileLocation(StartPoint.y, StartPoint.x) &&
		Row[StartPoint.y].Column[StartPoint.x] != ETileDesignation::TD_Wall) {
		if (!IsIntersection(StartPoint.y, StartPoint.x)) {
			TArray<FMazeRowData> CopyRow;
			CopyRow.Append(Row);
			CopyRow[StartPoint.y].Column[StartPoint.x] = ETileDesignation::TD_Visited;
			TArray<FIntPair> PathStack;
			PathStack.Add(StartPoint);

			if (StartDirection == EDirection::D_North && StartPoint.y - 1 > 0 && CopyRow[StartPoint.y - 1].Column[StartPoint.x] == ETileDesignation::TD_Path) {
				if (IsValidTileLocation(StartPoint.y, StartPoint.x - 1)) {
					CopyRow[StartPoint.y].Column[StartPoint.x - 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y, StartPoint.x + 1)) {
					CopyRow[StartPoint.y].Column[StartPoint.x + 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y + 1, StartPoint.x)) {
					CopyRow[StartPoint.y + 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}
			}
			else if (StartDirection == EDirection::D_East && StartPoint.x + 1 < MazeLengthInTiles && CopyRow[StartPoint.y].Column[StartPoint.x + 1] == ETileDesignation::TD_Path) {
				if (IsValidTileLocation(StartPoint.y, StartPoint.x - 1)) {
					CopyRow[StartPoint.y].Column[StartPoint.x - 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y - 1, StartPoint.x)) {
					CopyRow[StartPoint.y - 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y + 1, StartPoint.x)) {
					CopyRow[StartPoint.y + 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}
			}
			else if (StartDirection == EDirection::D_South && StartPoint.y + 1< MazeLengthInTiles && CopyRow[StartPoint.y + 1].Column[StartPoint.x] == ETileDesignation::TD_Path) {
				if (IsValidTileLocation(StartPoint.y, StartPoint.x - 1)) {
					CopyRow[StartPoint.y].Column[StartPoint.x - 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y, StartPoint.x + 1)) {
					CopyRow[StartPoint.y].Column[StartPoint.x + 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y - 1, StartPoint.x)) {
					CopyRow[StartPoint.y - 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}
			}
			else if (StartDirection == EDirection::D_West && StartPoint.x - 1 > 0 && CopyRow[StartPoint.y].Column[StartPoint.x - 1] == ETileDesignation::TD_Path) {
				if (IsValidTileLocation(StartPoint.y, StartPoint.x + 1)) {
					CopyRow[StartPoint.y].Column[StartPoint.x + 1] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y - 1, StartPoint.x)) {
					CopyRow[StartPoint.y - 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
				}
				if (IsValidTileLocation(StartPoint.y + 1, StartPoint.x)) {
					CopyRow[StartPoint.y + 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
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
				if (StackHead.y - 1 >= 0 && CopyRow[StackHead.y - 1].Column[StackHead.x] == ETileDesignation::TD_Path) {
					ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y - 1));
				}

				// Lower Neighbor
				if (StackHead.y + 1 < MazeLengthInTiles && CopyRow[StackHead.y + 1].Column[StackHead.x] == ETileDesignation::TD_Path) {
					ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y + 1));
				}

				// Left Neighbor
				if (StackHead.x - 1 >= 0 && CopyRow[StackHead.y].Column[StackHead.x - 1] == ETileDesignation::TD_Path) {
					ValidNeighbors.Add(FIntPair(StackHead.x - 1, StackHead.y));
				}

				// Right Neighbor
				if (StackHead.x + 1 < MazeLengthInTiles && CopyRow[StackHead.y].Column[StackHead.x + 1] == ETileDesignation::TD_Path) {
					ValidNeighbors.Add(FIntPair(StackHead.x + 1, StackHead.y));
				}

				if (ValidNeighbors.Num() != 0) {
					// Choose random valid neighbor
					StackHead = ValidNeighbors[FMath::RandRange(0, ValidNeighbors.Num() - 1)];
					CopyRow[StackHead.y].Column[StackHead.x] = ETileDesignation::TD_Visited;
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

void AMazeSegment::NextIntersectionBP(int32 StartPointX, int32 StartPointY, int32 & IntersectionX, int32 & IntersectionY, EDirection StartDirection, int32 MaxDistance) {
	FIntPair Intersection;
	NextIntersection(FIntPair(StartPointX, StartPointY), Intersection, StartDirection, MaxDistance);
	IntersectionX = Intersection.x;
	IntersectionY = Intersection.y;
}

bool AMazeSegment::PathHasIntersectionBP(TArray<FVector> Path, int32 & IntersectionX, int32 & IntersectionY) {
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
	//Left Border
	AActor* BorderWall = GetWorld()->SpawnActor(BorderClass);
	BorderWall->SetActorLocation(GetActorLocation() + FVector(0.f, 0.f, FloorHeight));
	BorderWall->SetActorScale3D(FVector(TileSize / 100.f, (float)((MazeLengthInTiles + 2) / 2) * TileSize / 100.f, OuterWallHeight / 100.f));

	BorderWall = GetWorld()->SpawnActor(BorderClass);
	BorderWall->SetActorLocation(GetActorLocation() + FVector(0.f, (float)((MazeLengthInTiles + 2) / 2 + 1) * TileSize, FloorHeight));
	BorderWall->SetActorScale3D(FVector(TileSize / 100.f, (float)((MazeLengthInTiles + 2) / 2) * TileSize / 100.f, OuterWallHeight / 100.f));

	//Right Border
	BorderWall = GetWorld()->SpawnActor(BorderClass);
	BorderWall->SetActorLocation(GetActorLocation() + FVector((float)(MazeLengthInTiles + 1) * TileSize, 0.f, FloorHeight));
	BorderWall->SetActorScale3D(FVector(TileSize / 100.f, (float)((MazeLengthInTiles + 2) / 2) * TileSize / 100.f, OuterWallHeight / 100.f));

	BorderWall = GetWorld()->SpawnActor(BorderClass);
	BorderWall->SetActorLocation(GetActorLocation() + FVector((float)(MazeLengthInTiles + 1) * TileSize, (float)((MazeLengthInTiles + 2) / 2 + 1) * TileSize, FloorHeight));
	BorderWall->SetActorScale3D(FVector(TileSize / 100.f, (float)((MazeLengthInTiles + 2) / 2) * TileSize / 100.f, OuterWallHeight / 100.f));

	//Top Border
	BorderWall = GetWorld()->SpawnActor(BorderClass);
	BorderWall->SetActorLocation(GetActorLocation() + FVector(TileSize, 0.f, FloorHeight));
	BorderWall->SetActorScale3D(FVector((float)(MazeLengthInTiles / 2) * TileSize / 100.f, TileSize / 100.f, OuterWallHeight / 100.f));

	BorderWall = GetWorld()->SpawnActor(BorderClass);
	BorderWall->SetActorLocation(GetActorLocation() + FVector((float)((MazeLengthInTiles + 2) / 2 + 1) * TileSize, 0.f, FloorHeight));
	BorderWall->SetActorScale3D(FVector((float)(MazeLengthInTiles / 2) * TileSize / 100.f, TileSize / 100.f, OuterWallHeight / 100.f));

	//Bottom Border
	BorderWall = GetWorld()->SpawnActor(BorderClass);
	BorderWall->SetActorLocation(GetActorLocation() + FVector(TileSize, (float)(MazeLengthInTiles + 1) * TileSize, FloorHeight));
	BorderWall->SetActorScale3D(FVector((float)(MazeLengthInTiles / 2) * TileSize / 100.f, TileSize / 100.f, OuterWallHeight / 100.f));

	BorderWall = GetWorld()->SpawnActor(BorderClass);
	BorderWall->SetActorLocation(GetActorLocation() + FVector((float)((MazeLengthInTiles + 2) / 2 + 1) * TileSize, (float)(MazeLengthInTiles + 1) * TileSize, FloorHeight));
	BorderWall->SetActorScale3D(FVector((float)(MazeLengthInTiles / 2) * TileSize / 100.f, TileSize / 100.f, OuterWallHeight / 100.f));




}

void AMazeSegment::SpawnFloor() {
	AActor* Floor = GetWorld()->SpawnActor(FloorClass);
	Floor->SetActorLocation(GetActorLocation());
	Floor->SetActorScale3D(FVector((float)(MazeLengthInTiles + 2) * TileSize / 100.f, (float)(MazeLengthInTiles + 2) * TileSize / 100.f, FloorHeight / 100.f));

}

void AMazeSegment::SpawnWalls() {
	AMazeWall* CurrentWall;
	float VisibilityOffset = 0.1f; // Keeps the ground from clipping with lowered walls
	for (int y = 0; y < MazeLengthInTiles; y++) {
		Row[y].ColumnWallRef.SetNum(MazeLengthInTiles);
		for (int x = 0; x < MazeLengthInTiles; x++) {
			if (Row[y].Column[x] == ETileDesignation::TD_Wall) {
				CurrentWall = Cast<AMazeWall>(GetWorld()->SpawnActor(WallClass));
				if (CurrentWall) {
					CurrentWall->SetActorLocation(GetActorLocation() + FVector((float)(x + 1) * TileSize, (float)(y + 1) * TileSize, FloorHeight - VisibilityOffset));
					CurrentWall->SetActorScale3D(FVector(TileSize / 100.f, TileSize / 100.f, InnerWallHeight / 100.f));
					Row[y].ColumnWallRef[x] = CurrentWall;
				}
			}
		}

	}
}

// Called every frame
void AMazeSegment::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMazeSegment::VectorArraytoIntPairArray(TArray<FVector> InputArray, TArray<FIntPair> & Result) {
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

//testString = FString::FromInt(testArray.Num());
/*for (auto& currentTile : WallRow.Column) {
testString = EnumPtr->GetEnumName((uint8)currentTile);
GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, testString);
}*/