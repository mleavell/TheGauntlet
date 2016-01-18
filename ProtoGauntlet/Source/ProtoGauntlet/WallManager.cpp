// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtoGauntlet.h"
#include "WallManager.h"


// Sets default values
AWallManager::AWallManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	FloorClippingPreventionOffset = 2.f;
	InitialMeshHeight = 100.f;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Walls = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Walls"));
	Walls->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void AWallManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWallManager::SpawnWalls_Implementation(AMazeSegment* OwningMaze)
{
	if (!MazeRef) {
		MazeRef = OwningMaze;
		if (MazeRef) {
			MazeInfo = MazeRef->GetMazeInfo();
			WallUnderGroundLevelZ = -(InitialMeshHeight / 2.f) - FloorClippingPreventionOffset;
			WallMidAirLevelZ = InitialMeshHeight + (InitialMeshHeight / 2.f);
			WallGroundLevelZ = InitialMeshHeight / 2.f;
			SetActorScale3D(MazeInfo.WallScale);
		}

		FVector MeshDecenteringOffset = FVector(InitialMeshHeight / 2.f, InitialMeshHeight / 2.f, WallGroundLevelZ);
		FVector LocationOffset = MeshDecenteringOffset;

		for (int32 WallRow = 0; WallRow < MazeInfo.MazeLengthInTiles; WallRow++) {
			for (int32 WallColumn = 0; WallColumn < MazeInfo.MazeLengthInTiles; WallColumn++) {
				LocationOffset = MeshDecenteringOffset;
				LocationOffset.X += (float)(WallColumn) * InitialMeshHeight;
				LocationOffset.Y += (float)(WallRow) * InitialMeshHeight;

				if (MazeRef->GetTileDesignationAt(WallRow, WallColumn) == ETileDesignation::TD_Path) {
					LocationOffset.Z = WallUnderGroundLevelZ;
				}

				Walls->AddInstance(FTransform(LocationOffset));
			}
		}
	}
}

bool AWallManager::StartLoweringWallAt(int32 WallRow, int32 WallColumn, float WallLowerInterval){
	bool LoweringStarted = false;
	if (!WallIsRaisingOrLoweringAt(WallRow, WallColumn) && !WallIsUnderGroundLevelAt(WallRow, WallColumn)) {
		FTransform InstanceTransform;
		Walls->GetInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform);
		StartingLocations.Add(InstanceTransform.GetLocation());

		MovingWallRowsAndColumns.Add(FIntPair(WallColumn, WallRow));
		WallMovementIntervals.Add(WallLowerInterval);
		EndingTime.Add(GetWorld()->TimeSeconds);
		WallMovementDirections.Add(EDirection::D_Down);
		LoweringStarted = true;
	}
	return LoweringStarted;
}

bool AWallManager::StartRaisingWallAt(int32 WallRow, int32 WallColumn, float WallRaiseInterval){
	bool RaisingStarted = false;
	if (!WallIsRaisingOrLoweringAt(WallRow, WallColumn) && !WallIsMidAirAt(WallRow, WallColumn)) {
		FTransform InstanceTransform;
		Walls->GetInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform);
		StartingLocations.Add(InstanceTransform.GetLocation());

		MovingWallRowsAndColumns.Add(FIntPair(WallColumn, WallRow));
		WallMovementIntervals.Add(WallRaiseInterval);
		EndingTime.Add(GetWorld()->TimeSeconds);
		WallMovementDirections.Add(EDirection::D_Up);
		RaisingStarted = true;
	}
	return RaisingStarted;
}

void AWallManager::TeleportWallUndergroundAt(int32 WallRow, int32 WallColumn){
	if (WallIsRaisingOrLoweringAt(WallRow, WallColumn)){
		int32 InstanceIndex = MovingWallRowsAndColumns.Find(FIntPair(WallColumn, WallRow));
		StartingLocations.RemoveAt(InstanceIndex);
		MovingWallRowsAndColumns.RemoveAt(InstanceIndex);
		WallMovementIntervals.RemoveAt(InstanceIndex);
		EndingTime.RemoveAt(InstanceIndex);
		WallMovementDirections.RemoveAt(InstanceIndex);
	}

	FTransform InstanceTransform;
	FVector NewLocation;
	Walls->GetInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform);
	NewLocation = InstanceTransform.GetLocation();
	NewLocation.Z = WallUnderGroundLevelZ;
	InstanceTransform.SetLocation(NewLocation);
	Walls->UpdateInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform);
}

void AWallManager::UpdateAllWallTransformsAtCurrentTime_Implementation() {

}
bool AWallManager::WallIsMidAirAt(int32 WallRow, int32 WallColumn){
	FTransform InstanceTransform;
	Walls->GetInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform);
	return FMath::IsNearlyEqual(InstanceTransform.GetLocation().Z, WallMidAirLevelZ, .001f);
}

bool AWallManager::WallIsGroundLevelAt(int32 WallRow, int32 WallColumn){
	FTransform InstanceTransform;
	Walls->GetInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform);
	return FMath::IsNearlyEqual(InstanceTransform.GetLocation().Z, WallGroundLevelZ, .001f);
}

bool AWallManager::WallIsRaisingOrLoweringAt(int32 WallRow, int32 WallColumn){
	return MovingWallRowsAndColumns.Find(FIntPair(WallColumn, WallRow)) != INDEX_NONE;
}

bool AWallManager::WallIsUnderGroundLevelAt(int32 WallRow, int32 WallColumn){
	FTransform InstanceTransform;
	Walls->GetInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform);
	return FMath::IsNearlyEqual(InstanceTransform.GetLocation().Z, WallUnderGroundLevelZ, .001f);

}

int32 AWallManager::GetInstanceIndexForWallAt(int32 WallRow, int32 WallColumn) {
	return (WallRow * MazeInfo.MazeLengthInTiles) + WallColumn;
}