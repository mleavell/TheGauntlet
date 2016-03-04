// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MazeSegment.h"
#include "SpiralLabyrinth.generated.h"

/**
 * 
 */
UCLASS()
class PROTOGAUNTLET_API ASpiralLabyrinth : public AMazeSegment
{
	GENERATED_BODY()

protected:

	void CreateMazeLayout();
	
	
	
};
