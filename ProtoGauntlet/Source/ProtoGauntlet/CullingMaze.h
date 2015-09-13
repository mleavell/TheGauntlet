// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MazeSegment.h"
#include "CullingMaze.generated.h"

/**
 * 
 */
UCLASS()
class PROTOGAUNTLET_API ACullingMaze : public AMazeSegment
{
	GENERATED_BODY()

		//void CreateMazeLayout();
protected:

	void InitialPillarRaise();
	
	void SpawnWalls();

	void SpawnBorders();
	
	void SpawnFloor();

	UPROPERTY(BlueprintReadOnly)
	TArray<AMazeWall*> StandingPillars;

	int32 PillarLayers;

	void LowerLayerOfPillars();
	
	void DominoWallsFromDirection();

	EDirection CurrentDominoDirection;
	
	FTimerHandle DominoHandle;

	int32 DominoEffectRow;

	int32 DominoEffectColumn;
};
