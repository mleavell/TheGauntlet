// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MyActor.h"
#include "MazeSegment.generated.h"

UCLASS()
class PROTOGAUNTLET_API AMazeSegment : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	bool IsCenterPiece;

	/** Only odd values. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	int32 MazeLengthInTiles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	float TileSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	float HalfTileSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	float FloorHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	float InnerWallHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	float OuterWallHeight;

	UPROPERTY(BlueprintReadOnly)
	TArray<FMazeRowData> Row;

	// Sets default values for this actor's properties
	AMazeSegment();

	void ChangeMazeParameters(int32 MazeLengthInTiles, float TileSize, float FloorHeight, float InnerWallHeight, float OuterWallHeight);

	bool IsValidTileLocation(int32 TileRow, int32 TileColumn);

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
		void FindPathBetweenPoints(FIntPair StartPoint, FIntPair EndPoint, TArray<FIntPair> & Result, EDirection StartDirection = EDirection::D_None);

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
		void GetAllTilesInSection(FIntPair StartPoint, TArray<FIntPair> & Result, EDirection StartDirection);

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
		void CreateRandomPathFromStartPoint(FIntPair StartPoint, TArray<FIntPair> & Result, int32 PathLength = 10);

	UFUNCTION(BlueprintCallable, Category = "Dimensions")
	ETileDesignation GetTileDesignationAt(int32 TileRow, int32 TileColumn);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void PostInitProperties() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

protected:

	virtual void CreateMazeLayout();

	virtual void SpawnWalls();
	
	virtual void SpawnFloor();

	virtual void SpawnBorders();

	void CalculateValues();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> FloorClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> BorderClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMazeWall> WallClass;
	
};
