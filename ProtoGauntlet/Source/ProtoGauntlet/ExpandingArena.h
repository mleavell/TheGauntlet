// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MazeSegment.h"
#include "ExpandingArena.generated.h"

/**
 * 
 */
UCLASS()
class PROTOGAUNTLET_API AExpandingArena : public AMazeSegment
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Raise and Lower")
	void ChangeDesiredLayer(int32 ChosenLayer);

	UFUNCTION(BlueprintCallable, Category = "Raise and Lower")
	int32 GetCurrentLayerOfWallsLowered();

protected:

	AExpandingArena();

	void CreateMazeLayout();

	void SpawnWalls();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	TArray<AMazeWall*> RowWalls;
	
	TArray<AMazeWall*> ColumnWalls;

	int32 DesiredLayerOfWallsLowered;

	int32 CurrentLayerOfWallsLowered;
	
	void LowerLayerOfWalls();

	void RaiseLayerOfWalls();
};
