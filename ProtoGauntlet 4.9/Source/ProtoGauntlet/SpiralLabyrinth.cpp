// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtoGauntlet.h"
#include "SpiralLabyrinth.h"

void ASpiralLabyrinth::CreateMazeLayout() {
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
	
	TArray<FIntPair> ValidNeighbors;
	FIntPair StackHead;
	Row[0].Column[0] = ETileDesignation::TD_Path;
	int32 CellNum = (MazeLengthInTiles / 2 + 1) * (MazeLengthInTiles / 2 + 1) - 1;
	EDirection PathDirection = EDirection::D_East;
	
	while (CellNum > 0) {
		if (PathDirection == EDirection::D_East){
			if (GetTileDesignationAt(StackHead.y, StackHead.x + 2) != ETileDesignation::TD_OutOfBounds && GetTileDesignationAt(StackHead.y, StackHead.x + 2 ) != ETileDesignation::TD_Path) {
				Row[StackHead.y].Column[StackHead.x + 1] = ETileDesignation::TD_Path;
				Row[StackHead.y].Column[StackHead.x + 2] = ETileDesignation::TD_Path;
				CellNum -= 1;
				StackHead = FIntPair(StackHead.x + 2, StackHead.y);
			} else {
				PathDirection = EDirection::D_South;
			}
		}

		if (PathDirection == EDirection::D_South){
			if (GetTileDesignationAt(StackHead.y + 2, StackHead.x) != ETileDesignation::TD_OutOfBounds && GetTileDesignationAt(StackHead.y + 2, StackHead.x) != ETileDesignation::TD_Path) {
				Row[StackHead.y + 1].Column[StackHead.x] = ETileDesignation::TD_Path;
				Row[StackHead.y + 2].Column[StackHead.x] = ETileDesignation::TD_Path;
				CellNum -= 1;
				StackHead = FIntPair(StackHead.x, StackHead.y + 2);
			}
			else {
				PathDirection = EDirection::D_West;
			}
		}

		if (PathDirection == EDirection::D_West){
			if (GetTileDesignationAt(StackHead.y, StackHead.x - 2) != ETileDesignation::TD_OutOfBounds && GetTileDesignationAt(StackHead.y, StackHead.x - 2) != ETileDesignation::TD_Path) {
				Row[StackHead.y].Column[StackHead.x - 1] = ETileDesignation::TD_Path;
				Row[StackHead.y].Column[StackHead.x - 2] = ETileDesignation::TD_Path;
				CellNum -= 1;
				StackHead = FIntPair(StackHead.x - 2, StackHead.y);
			} else {
				PathDirection = EDirection::D_North;
			}
		}

		if (PathDirection == EDirection::D_North){
			if (GetTileDesignationAt(StackHead.y - 2, StackHead.x) != ETileDesignation::TD_OutOfBounds && GetTileDesignationAt(StackHead.y - 2, StackHead.x) != ETileDesignation::TD_Path) {
				Row[StackHead.y - 1].Column[StackHead.x] = ETileDesignation::TD_Path;
				Row[StackHead.y - 2].Column[StackHead.x] = ETileDesignation::TD_Path;
				CellNum -= 1;
				StackHead = FIntPair(StackHead.x, StackHead.y - 2);
			}
			else {
				PathDirection = EDirection::D_East;
			}
		}
	}

}

