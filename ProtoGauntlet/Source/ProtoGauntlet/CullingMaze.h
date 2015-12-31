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

public:
		ACullingMaze();

		UFUNCTION(BlueprintCallable, Category = "Pathfinding")
		void GetTileIndexAtLocation(FVector Location, int32& TileRow, int32& TileColumn);

protected:

	EDirection CurrentDominoFromDirection;

	int32 CurrentWallColumn;

	int32 CurrentWallRow;

	int32 DominoEffectColumn;

	int32 DominoEffectRow;

	FTimerHandle DominoHandle;

	int32 EastOuterMostPillarColumn;

	int32 JustAfterHalfway;

	int32 JustBeforeHalfway;

	int32 MinimumPillarLayers;

	int32 NorthernOuterMostPillarRow;

	int32 PillarLayers;

	int32 SouthernOuterMostPillarRow;

	int32 SpaceBetweenPillarLayers;

	int32 WestOuterMostPillarColumn;

	UPROPERTY(BlueprintReadOnly)
	TArray<AMazeWall*> StandingPillars;

	float TimeBetweenDirectionChange;

	float TimeBetweenDominoEffectBetweenToWalls;
	
	virtual void BeginPlay() override;

	void ChangeDominoDirection();

	void DominoWallsFromDirection();

	void InitialPillarRaise();

	bool IsBehindStandingPillar(int32 WallRow, int32 WallColumn);

	bool IsStandingPillar(int32 WallRow, int32 WallColumn);

	void LowerLayerOfPillars();

	void RaisePillarsInNorthEastQuadrant();

	void RaisePillarsInNorthWestQuadrant();

	void RaisePillarsInSouthEastQuadrant();

	void RaisePillarsInSouthWestQuadrant();

	void SpawnBorders();

	void SpawnFloor();
	
	void SpawnWalls();

	void SwitchToNextColumnOfWalls();

	void SwitchToNextRowOfWalls();
};
