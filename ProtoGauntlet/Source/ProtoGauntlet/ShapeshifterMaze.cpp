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
	for (int y = 0; y < MazeLengthInTiles; y++) {
		for (int x = 0; x < MazeLengthInTiles; x++) {
			if (y % 2 == 0 && x % 2 == 0) {
				Row[y].Column[x] = ETileDesignation::TD_Cell;
			}
			else{
				Row[y].Column[x] = ETileDesignation::TD_Wall;
			}
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