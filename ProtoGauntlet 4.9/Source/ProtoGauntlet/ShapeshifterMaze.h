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

	AShapeshifterMaze();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shapeshift")
		float PostDepowerDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shapeshift")
	float ShapeshiftDelay;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Shapeshift")
		void LowerInactiveWalls();

	UFUNCTION(BlueprintCallable, Category = "Shapeshift")
		void RaiseAllWalls();

	UFUNCTION(BlueprintImplementableEvent)
		void Shapeshift();

	UFUNCTION(BlueprintCallable, Category = "Shapeshift")
	void ShuffleMazeLayout();

	UFUNCTION(BlueprintCallable, Category = "Shapeshift")
	void SpawnWalls();
	
	
};
