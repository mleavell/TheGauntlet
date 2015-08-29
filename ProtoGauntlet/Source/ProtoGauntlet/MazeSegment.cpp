// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtoGauntlet.h"
#include "MazeSegment.h"


// Sets default values
AMazeSegment::AMazeSegment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	IsCenterPiece = false;
	TileSize = 400.f;
	MazeLenghInTiles = 41;
	FloorHeight = 100.f;
	InnerWallHeight = 600.f;
	OuterWallHeight = 800.f;

}

void AMazeSegment::PostInitProperties()
{
	Super::PostInitProperties();

	CalculateValues();
}

void AMazeSegment::CalculateValues()
{
	HalfTileSize = TileSize / 2.f;
	if (MazeLenghInTiles % 2 == 0) {
		MazeLenghInTiles++;
	}
}

#if WITH_EDITOR
void AMazeSegment::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	CalculateValues();

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

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
	
	

}

// Called every frame
void AMazeSegment::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMazeSegment::ChangeMazeParameters(int32 MazeLenghInTiles, float TileSize, float FloorHeight, float InnerWallHeight, float OuterWallHeight) 
{
	this->TileSize = TileSize;
	this->HalfTileSize = TileSize / 2.f;
	this->MazeLenghInTiles = MazeLenghInTiles;
	this->FloorHeight = FloorHeight;
	this->InnerWallHeight = InnerWallHeight;
	this->OuterWallHeight = OuterWallHeight;
}

void AMazeSegment::CreateMazeLayout() {
	FMazeRowData PathRow;
	if (!IsCenterPiece) {
		FMazeRowData CellRow;
		FMazeRowData WallRow;

		WallRow.Column.Init(ETileDesignation::TD_Wall, MazeLenghInTiles);
		for (int32 index = 0; index < MazeLenghInTiles; index++) {
			if (index % 2 == 1) {
				CellRow.Column.Add(ETileDesignation::TD_Wall);
			}
			else {
				CellRow.Column.Add(ETileDesignation::TD_Cell);
			}

		}

		for (int32 index = 0; index < MazeLenghInTiles; index++) {
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
			if (StackHead.y + 2 < MazeLenghInTiles && Row[StackHead.y + 2].Column[StackHead.x] == ETileDesignation::TD_Cell) {
				ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y + 2));
			}

			// Left Neighbor
			if (StackHead.x - 2 >= 0 && Row[StackHead.y].Column[StackHead.x - 2] == ETileDesignation::TD_Cell) {
				ValidNeighbors.Add(FIntPair(StackHead.x - 2, StackHead.y));
			}

			// Right Neighbor
			if (StackHead.x + 2 < MazeLenghInTiles && Row[StackHead.y].Column[StackHead.x + 2] == ETileDesignation::TD_Cell) {
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
	} else {
		PathRow.Column.Init(ETileDesignation::TD_Path, MazeLenghInTiles);
		for (int32 index = 0; index < MazeLenghInTiles; index++) {
			Row.Add(PathRow);

		}
	}

}

ETileDesignation AMazeSegment::GetTileDesignationAt(int32 TileRow, int32 TileColumn) {
	if (TileRow >= 0 && TileRow < MazeLenghInTiles && TileColumn >= 0 && TileColumn < MazeLenghInTiles) {
		return Row[TileRow].Column[TileColumn];
	}

	return ETileDesignation::TD_OutOfBounds;
}

void AMazeSegment::FindPathBetweenPoints(FIntPair StartPoint, FIntPair EndPoint, TArray<FIntPair> & Result, EDirection StartDirection) {
	if (IsValidTileLocation(StartPoint.y, StartPoint.x) && Row[StartPoint.y].Column[StartPoint.x] != ETileDesignation::TD_Wall && Row[EndPoint.y].Column[EndPoint.x] != ETileDesignation::TD_Wall) {
		TArray<FMazeRowData> CopyRow;
		/*for (auto& ColumnData : Row) {
			CopyRow.Emplace(ColumnData);

		}*/
		CopyRow.Append(Row);
		CopyRow[StartPoint.y].Column[StartPoint.x] = ETileDesignation::TD_Visited;
		Result.Add(StartPoint);

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
			else if (StartDirection == EDirection::D_East && StartPoint.x < MazeLenghInTiles && CopyRow[StartPoint.y].Column[StartPoint.x + 1] == ETileDesignation::TD_Path) {
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
			else if (StartDirection == EDirection::D_South && StartPoint.y < MazeLenghInTiles && CopyRow[StartPoint.y + 1].Column[StartPoint.x] == ETileDesignation::TD_Path) {
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
			} else {
				Result.Pop();
			}
		}
		
		TArray<FIntPair> ValidNeighbors;
		FIntPair StackHead = StartPoint;
		while (Result.Num() != 0 && Result.Last() != EndPoint) {
			ValidNeighbors.SetNum(0);

			// Upper Neighbor
			if (StackHead.y - 1 >= 0 && CopyRow[StackHead.y - 1].Column[StackHead.x] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y - 1));
			}

			// Lower Neighbor
			if (StackHead.y + 1 < MazeLenghInTiles && CopyRow[StackHead.y + 1].Column[StackHead.x] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y + 1));
			}

			// Left Neighbor
			if (StackHead.x - 1 >= 0 && CopyRow[StackHead.y].Column[StackHead.x - 1] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x - 1, StackHead.y));
			}

			// Right Neighbor
			if (StackHead.x + 1 < MazeLenghInTiles && CopyRow[StackHead.y].Column[StackHead.x + 1] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x + 1, StackHead.y));
			}

			if (ValidNeighbors.Num() != 0) {
				// Choose random valid neighbor
				StackHead = ValidNeighbors[FMath::RandRange(0, ValidNeighbors.Num() - 1)];
				CopyRow[StackHead.y].Column[StackHead.x] = ETileDesignation::TD_Visited;
				Result.Push(StackHead);
			} else {
				Result.Pop();
				if (Result.Num() != 0) {
					StackHead = Result.Last();
				}
			}
		}
	}
};

void AMazeSegment::GetAllTilesInSection(FIntPair StartPoint, TArray<FIntPair> & Result, EDirection StartDirection) {
	if (IsValidTileLocation(StartPoint.y, StartPoint.x) && Row[StartPoint.y].Column[StartPoint.x] != ETileDesignation::TD_Wall) {
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
		} else if (StartDirection == EDirection::D_East && StartPoint.x < MazeLenghInTiles && CopyRow[StartPoint.y].Column[StartPoint.x + 1] == ETileDesignation::TD_Path) {
			if (IsValidTileLocation(StartPoint.y, StartPoint.x - 1)) {
				CopyRow[StartPoint.y].Column[StartPoint.x - 1] = ETileDesignation::TD_Visited;
			}
			if (IsValidTileLocation(StartPoint.y - 1, StartPoint.x)) {
				CopyRow[StartPoint.y - 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
			}
			if (IsValidTileLocation(StartPoint.y + 1, StartPoint.x)) {
				CopyRow[StartPoint.y + 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
			}
		} else if (StartDirection == EDirection::D_South && StartPoint.y < MazeLenghInTiles && CopyRow[StartPoint.y + 1].Column[StartPoint.x] == ETileDesignation::TD_Path) {
			if (IsValidTileLocation(StartPoint.y, StartPoint.x - 1)) {
				CopyRow[StartPoint.y].Column[StartPoint.x - 1] = ETileDesignation::TD_Visited;
			}
			if (IsValidTileLocation(StartPoint.y, StartPoint.x + 1)) {
				CopyRow[StartPoint.y].Column[StartPoint.x + 1] = ETileDesignation::TD_Visited;
			}
			if (IsValidTileLocation(StartPoint.y - 1, StartPoint.x)) {
				CopyRow[StartPoint.y - 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
			}
		} else if (StartDirection == EDirection::D_West && StartPoint.x > 0 && CopyRow[StartPoint.y].Column[StartPoint.x - 1] == ETileDesignation::TD_Path) {
			if (IsValidTileLocation(StartPoint.y, StartPoint.x + 1)) {
				CopyRow[StartPoint.y].Column[StartPoint.x + 1] = ETileDesignation::TD_Visited;
			}
			if (IsValidTileLocation(StartPoint.y - 1, StartPoint.x)) {
				CopyRow[StartPoint.y - 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
			}
			if (IsValidTileLocation(StartPoint.y + 1, StartPoint.x)) {
				CopyRow[StartPoint.y + 1].Column[StartPoint.x] = ETileDesignation::TD_Visited;
			}
		} else {
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
			if (StackHead.y + 1 < MazeLenghInTiles && CopyRow[StackHead.y + 1].Column[StackHead.x] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y + 1));
			}

			// Left Neighbor
			if (StackHead.x - 1 >= 0 && CopyRow[StackHead.y].Column[StackHead.x - 1] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x - 1, StackHead.y));
			}

			// Right Neighbor
			if (StackHead.x + 1 < MazeLenghInTiles && CopyRow[StackHead.y].Column[StackHead.x + 1] == ETileDesignation::TD_Path) {
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
			if (StackHead.y + 1 < MazeLenghInTiles && CopyRow[StackHead.y + 1].Column[StackHead.x] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x, StackHead.y + 1));
			}

			// Left Neighbor
			if (StackHead.x - 1 >= 0 && CopyRow[StackHead.y].Column[StackHead.x - 1] == ETileDesignation::TD_Path) {
				ValidNeighbors.Add(FIntPair(StackHead.x - 1, StackHead.y));
			}

			// Right Neighbor
			if (StackHead.x + 1 < MazeLenghInTiles && CopyRow[StackHead.y].Column[StackHead.x + 1] == ETileDesignation::TD_Path) {
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

bool AMazeSegment::IsValidTileLocation(int32 TileRow, int32 TileColumn) {
	return TileColumn >= 0 && TileRow >= 0 && TileColumn < MazeLenghInTiles && TileRow < MazeLenghInTiles;
}

// uint8 test = (uint8)ETileDesignation::TD_Cell;
//const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ETileDesignation"), true);
//FString testString = EnumPtr->GetEnumName(test);
////FText testText = FText::FromString(testString);

//TArray<ETileDesignation> testArray;
//for (int index = 0; index < MazeLenghInTiles; index++) {
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