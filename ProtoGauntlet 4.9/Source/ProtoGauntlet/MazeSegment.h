// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MyActor.h"
#include "MazeSegment.generated.h"

class AWallManager;

USTRUCT(BlueprintType)
struct FMazeInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze Info")
	float FloorHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze Info")
		int32 MazeLengthInTiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze Info")
		int32 TileSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze Info")
		FVector WallScale;

	FMazeInfo() {

	}

	FMazeInfo(int32 MazeLengthInTiles, float FloorHeight, int32 TileSize, FVector WallScale)
	{
		this->MazeLengthInTiles = MazeLengthInTiles;
		this->FloorHeight = FloorHeight;
		this->TileSize = TileSize;
		this->WallScale = WallScale;
	}
};

UCLASS()
class PROTOGAUNTLET_API AMazeSegment : public AActor
{
	GENERATED_BODY()
	
public:	
	UFUNCTION(BlueprintPure, Category = "Pathfinding")
	bool AboveGroundLevelWalls(AActor* InputActor);

	// Sets default values for this actor's properties
	AMazeSegment();

	void ChangeMazeParameters(int32 MazeLengthInTiles, float TileSize, float FloorHeight, float InnerWallHeight, float OuterWallHeight);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	void CreateRandomPathFromStartPoint(FIntPair StartPoint, TArray<FIntPair>& Result, int32 DesiredPathLength = 10);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	void CreateRandomPathFromStartPointBP(int32 StartPointX, int32 StartPointY, TArray<FVector>& Result, int32 PathLength = 10);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	void ExtractCorners(TArray<FIntPair> InputArray, TArray<FIntPair>& Result);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	void ExtractCornersBP(TArray<FVector> InputArray, TArray<FVector>& Result);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	void FindPathBetweenPoints(FIntPair StartPoint, FIntPair EndPoint, TArray<FIntPair>& Path, EDirection StartDirection = EDirection::D_None);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	void FindPathBetweenPointsBP(int32 StartPointX, int32 StartPointY, int32 EndPointX, int32 EndPointY, TArray<FVector>& Path, EDirection StartDirection = EDirection::D_None);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	void GetAllTilesInSection(FIntPair StartPoint, TArray<FIntPair>& Result, EDirection StartDirection);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	void GetDirectionsFromVectorArray(TArray<FVector> PathArray, TArray<uint8>& DirectionArray);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	void GetLocationOfTile(FVector& Location, int32 TileRow, int32 TileColumn);

	FMazeInfo GetMazeInfo();

	int32 GetMazeLengthInTiles();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	bool GetPathfindingActive();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	ETileDesignation GetTileDesignationAt(int32 TileRow, int32 TileColumn);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	virtual void GetTileIndexAtLocation(FVector Location, int32& TileRow, int32& TileColumn);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	void IntPairArraytoVectorArray(TArray<FIntPair> InputArray, TArray<FVector>& Result);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	bool IsCorner(int32 TileRow, int32 TileColumn);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	bool IsIntersection(int32 TileRow, int32 TileColumn);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	bool IsValidTileLocation(int32 TileRow, int32 TileColumn);

	UPROPERTY(BlueprintReadWrite, Category = "Pathfinding")
	bool NavMeshReady;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	void NextIntersection(FIntPair StartPoint, FIntPair& Intersection, EDirection StartDirection = EDirection::D_North, int32 MaxDistance = 5);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	void NextIntersectionBP(int32 StartPointX, int32 StartPointY, int32& IntersectionX, int32& IntersectionY, EDirection StartDirection = EDirection::D_North, int32 MaxDistance = 5);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	bool PathHasIntersectionBP(TArray<FVector> Path, int32& IntersectionX, int32& IntersectionY);

	FIntPair RandomTileThatStartsAsCell();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	bool TileIsWall(int32 TileRow, int32 TileColumn);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	void VectorArraytoIntPairArray(TArray<FVector> InputArray, TArray<FIntPair>& Result);

protected:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> BorderClass;

	float CentimetersToWallSizeScaleRatio;

	int32 EastBorder;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> FloorClass;

	float FloorClippingPreventionOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		float FloorHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		float HalfTileSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		float InnerWallHeight;

	float InnerWallHeightScale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		bool HasMaze;

	/** Only odd values. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		int32 MazeLengthInTiles;

	int32 NorthBorder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		float OuterWallHeight;

	float OuterWallHeightScale;

	bool PathfindingActive;

	UPROPERTY(BlueprintReadOnly)
		TArray<FMazeRowData> Row;

	int32 SouthBorder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		float TileSize;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMazeWall> WallClass;

	int32 WestBorder;
	
	float WallLengthScale;

	UPROPERTY(EditDefaultsOnly, Category = "Wall Management")
	TSubclassOf<AWallManager> WallManagementClass;

	UPROPERTY(BlueprintReadOnly, Category = "Wall Management")
	AWallManager* WallManagerRef;

	UPROPERTY(BlueprintReadOnly, Category = "Dimensions")
	FVector WallScale;

	float WallThicknessScale;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void CalculateValues();

	FIntPair ChooseRandomValidNeighbor(TArray<FIntPair>& ValidNeighbors);

	void ConnectCurrentTileToPath(TArray<FMazeRowData>& Row, TArray<FIntPair>& Path, FIntPair CurrentTile, bool CreatingMazeLayout = false);

	virtual void CreateMazeLayout();

	TArray<FIntPair>& GetValidNeighborsForContinuedPathCreation(TArray<FMazeRowData>& Row, FIntPair CurrentTileInPath, bool CreatingMazeLayout = false);

	void FormatMazeDataArrayForMazeGeneration();

	void InitializeWallScaleRatios();

	void MarkSurroundingTilesAsVisitedExcept(EDirection UnvisitedDirection, FIntPair CurrentTile, TArray<FMazeRowData>& CopyOfRow);

	virtual void PostInitProperties() override;

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);

	virtual void SpawnBorders();
	
	virtual void SpawnFloor();

	virtual void SpawnWalls();

	void ResetMazeLayout();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
};
