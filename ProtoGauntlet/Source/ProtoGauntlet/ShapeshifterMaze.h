// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MazeSegment.h"
#include "ShapeshifterMaze.generated.h"

/**
 * 
 */
UCLASS()
class PROTOGAUNTLET_API AShapeshifterMaze : public AMazeSegment
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shapeshift")
	float ShapeshiftDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shapeshift")
	float PostDepowerDelay;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Shapeshift")
	void RaiseAllWalls();

	UFUNCTION(BlueprintCallable, Category = "Shapeshift")
	void ShuffleMazeLayout();

	UFUNCTION(BlueprintCallable, Category = "Shapeshift")
	void LowerInactiveWalls();

	UFUNCTION(BlueprintCallable, Category = "Shapeshift")
	void SpawnWalls();

	UFUNCTION(BlueprintImplementableEvent)
	void Shapeshift();

	AShapeshifterMaze();
	
	
};
