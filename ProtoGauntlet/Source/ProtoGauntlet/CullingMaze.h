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
	
	void SpawnWalls();

	void SpawnFloor();

	void SpawnBorders();
	
	
};
