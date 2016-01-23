// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MazeSegment.h"
#include "WallManager.generated.h"

UCLASS()
class PROTOGAUNTLET_API AWallManager : public AActor
{
	GENERATED_BODY()
	
public:	

	AMazeSegment* MazeRef;

	// Sets default values for this actor's properties
	AWallManager();

	UFUNCTION(BlueprintPure, Category = "Wall Management")
	int32 GetWallCount();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wall Management")
	void SpawnWalls(AMazeSegment* OwningMaze);

	UFUNCTION(BlueprintCallable, Category = "Wall Management")
	bool StartLoweringWallAt(int32 WallRow, int32 WallColumn, float WallLowerInterval);

	UFUNCTION(BlueprintCallable, Category = "Wall Management")
	bool StartRaisingWallAt(int32 WallRow, int32 WallColumn, float WallRaiseInterval);

	UFUNCTION(BlueprintCallable, Category = "Wall Management")
	void TeleportWallGroundLevelAt(int32 WallRow, int32 WallColumn);

	UFUNCTION(BlueprintCallable, Category = "Wall Management")
	void TeleportWallMidAirAt(int32 WallRow, int32 WallColumn);

	UFUNCTION(BlueprintCallable, Category = "Wall Management")
	void TeleportWallUndergroundAt(int32 WallRow, int32 WallColumn);

	UFUNCTION(BlueprintPure, Category = "Wall Management")
	bool WallIsGroundLevelAt(int32 WallRow, int32 WallColumn);

	UFUNCTION(BlueprintPure, Category = "Wall Management")
	bool WallIsMidAirAt(int32 WallRow, int32 WallColumn);

	UFUNCTION(BlueprintPure, Category = "Wall Management")
	bool WallIsRaisingOrLoweringAt(int32 WallRow, int32 WallColumn);

	UFUNCTION(BlueprintPure, Category = "Wall Management")
	bool WallIsUnderGroundLevelAt(int32 WallRow, int32 WallColumn);

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<float> EndingTime;

	float FloorClippingPreventionOffset;

	UFUNCTION(BlueprintPure, Category = "Wall Management")
	int32 GetInstanceIndexForWallAt(int32 WallRow, int32 WallColumn);

	float InitialMeshHeight;

	FMazeInfo MazeInfo;

	TArray<FIntPair> MovingWallRowsAndColumns;

	TArray<FVector> StartingLocations;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wall Management")
	void UpdateAllWallTransformsAtCurrentTime();

	float WallGroundLevelZ;

	float WallMidAirLevelZ;

	TArray<EDirection> WallMovementDirections;

	TArray<float> WallMovementIntervals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* Walls;

	float WallUnderGroundLevelZ;
	
};