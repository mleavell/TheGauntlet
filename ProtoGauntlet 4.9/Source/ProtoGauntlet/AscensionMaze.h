// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MazeSegment.h"
#include "AscensionMaze.generated.h"

/**
 * 
 */
UCLASS()
class PROTOGAUNTLET_API AAscensionMaze : public AMazeSegment
{
	GENERATED_BODY()

public:


protected:

	void CreateMazeLayout();

	void SpawnBorders();

	void SpawnFloor();

	void SpawnWalls();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Raise and Lower")
	void Ascend();

private:

	TArray<AMazeWall*> EastWalls;

	TArray<AMazeWall*> NorthWalls;

	TArray<AMazeWall*> SouthWalls;

	TArray<AMazeWall*> WestWalls;

	AMazeWall* Centerpiece;

	int32 LevelOfAscension;

	int32 NumberOfLayers;

	int32 NumberOfAscensions;

	void Descend();

	
};
