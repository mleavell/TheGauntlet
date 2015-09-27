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
	// Sets default values for this actor's properties
	AMazeSegment();

	void ChangeMazeParameters(int32 MazeLengthInTiles, float TileSize, float FloorHeight, float InnerWallHeight, float OuterWallHeight);

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
	bool IsValidTileLocation(int32 TileRow, int32 TileColumn);

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
	bool IsIntersection(int32 TileRow, int32 TileColumn);

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
		void FindPathBetweenPoints(FIntPair StartPoint, FIntPair EndPoint, TArray<FIntPair> & Path, EDirection StartDirection = EDirection::D_None);

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
		void GetAllTilesInSection(FIntPair StartPoint, TArray<FIntPair> & Result, EDirection StartDirection);

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
		void NextIntersection(FIntPair StartPoint, FIntPair & Intersection, EDirection StartDirection = EDirection::D_North, int32 MaxDistance = 5);

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
		void CreateRandomPathFromStartPoint(FIntPair StartPoint, TArray<FIntPair> & Result, int32 PathLength = 10);

	UFUNCTION(BlueprintCallable, Category = "Dimensions")
	ETileDesignation GetTileDesignationAt(int32 TileRow, int32 TileColumn);

protected:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> BorderClass; 
	
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> FloorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		float FloorHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		float HalfTileSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		float InnerWallHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		bool IsCenterPiece;

	/** Only odd values. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		int32 MazeLengthInTiles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		float OuterWallHeight;

	bool PathfindingActive;

	UPROPERTY(BlueprintReadOnly)
		TArray<FMazeRowData> Row;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		float TileSize;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AMazeWall> WallClass;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void CalculateValues();

	virtual void CreateMazeLayout();

	virtual void PostInitProperties() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void SpawnBorders();
	
	virtual void SpawnFloor();

	virtual void SpawnWalls();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
};
