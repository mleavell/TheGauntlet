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
	WallMovementUpdatesPerSecond = 30;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Walls = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Walls"));
	Walls->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void AWallManager::BeginPlay()
{
	Super::BeginPlay();
}

int32 AWallManager::GetWallCount() {
	return Walls->GetInstanceCount();

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


			FVector MeshDecenteringOffset = FVector(InitialMeshHeight / 2.f, InitialMeshHeight / 2.f, WallGroundLevelZ);
			FVector LocationOffset = MeshDecenteringOffset;

			for (int32 WallRow = 0; WallRow < MazeInfo.MazeLengthInTiles; WallRow++) {
				for (int32 WallColumn = 0; WallColumn < MazeInfo.MazeLengthInTiles; WallColumn++) {
					LocationOffset = MeshDecenteringOffset;
					LocationOffset.X += (float)(WallColumn)* InitialMeshHeight;
					LocationOffset.Y += (float)(WallRow)* InitialMeshHeight;

					if (MazeRef->GetTileDesignationAt(WallRow, WallColumn) == ETileDesignation::TD_Path) {
						LocationOffset.Z = WallUnderGroundLevelZ;
					}

					Walls->AddInstance(FTransform(LocationOffset));
				}
			}

			FTimerHandle WallMoveTimer;
			GetWorldTimerManager().SetTimer(WallMoveTimer, this,
				&AWallManager::UpdateAllWallTransformsAtCurrentTime, 1.f / ((float)WallMovementUpdatesPerSecond), true);
		}
	}
}

bool AWallManager::StartLoweringWallAt(int32 WallRow, int32 WallColumn, float WallLowerInterval){
	bool LoweringStarted = false;
	if (!WallIsRaisingOrLoweringAt(WallRow, WallColumn) && !WallIsUnderGroundLevelAt(WallRow, WallColumn)) {
		FTransform InstanceTransform;
		Walls->GetInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform);
		FVector CurrentWallLocation = InstanceTransform.GetLocation();
		StartingLocations.Add(CurrentWallLocation);
		CurrentWallLocation.Z = WallIsGroundLevelAt(WallRow, WallColumn) ? WallUnderGroundLevelZ : WallGroundLevelZ;
		TargetLocations.Add(CurrentWallLocation);

		MovingWallRowsAndColumns.Add(FIntPair(WallColumn, WallRow));
		WallMovementIntervals.Add(WallLowerInterval);
		WallMoveBeginTime.Add(GetWorld()->TimeSeconds);
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
		FVector CurrentWallLocation = InstanceTransform.GetLocation();
		StartingLocations.Add(CurrentWallLocation);
		CurrentWallLocation.Z = WallIsGroundLevelAt(WallRow, WallColumn) ? WallMidAirLevelZ : WallGroundLevelZ;
		TargetLocations.Add(CurrentWallLocation);

		MovingWallRowsAndColumns.Add(FIntPair(WallColumn, WallRow));
		WallMovementIntervals.Add(WallRaiseInterval);
		WallMoveBeginTime.Add(GetWorld()->TimeSeconds);
		WallMovementDirections.Add(EDirection::D_Up);
		RaisingStarted = true;
	}
	return RaisingStarted;
}

void AWallManager::StopMovingWallAt(int32 WallRow, int32 WallColumn) {
	int32 InstanceIndex = MovingWallRowsAndColumns.Find(FIntPair(WallColumn, WallRow));
	StartingLocations.RemoveAt(InstanceIndex);
	MovingWallRowsAndColumns.RemoveAt(InstanceIndex);
	WallMovementIntervals.RemoveAt(InstanceIndex);
	WallMoveBeginTime.RemoveAt(InstanceIndex);
	WallMovementDirections.RemoveAt(InstanceIndex);
	TargetLocations.RemoveAt(InstanceIndex);
}

void AWallManager::TeleportWallGroundLevelAt(int32 WallRow, int32 WallColumn) {
	if (WallIsRaisingOrLoweringAt(WallRow, WallColumn)){
		StopMovingWallAt(WallRow, WallColumn);
	}

	FTransform InstanceTransform;
	FVector NewLocation;
	Walls->GetInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform);
	NewLocation = InstanceTransform.GetLocation();
	NewLocation.Z = WallGroundLevelZ;
	InstanceTransform.SetLocation(NewLocation);
	Walls->UpdateInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform, false, true);
}

void AWallManager::TeleportWallMidAirAt(int32 WallRow, int32 WallColumn) {
	if (WallIsRaisingOrLoweringAt(WallRow, WallColumn)){
		StopMovingWallAt(WallRow, WallColumn);
	}

	FTransform InstanceTransform;
	FVector NewLocation;
	Walls->GetInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform);
	NewLocation = InstanceTransform.GetLocation();
	NewLocation.Z = WallMidAirLevelZ;
	InstanceTransform.SetLocation(NewLocation);
	Walls->UpdateInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform, false, true);
}

void AWallManager::TeleportWallUndergroundAt(int32 WallRow, int32 WallColumn){
	if (WallIsRaisingOrLoweringAt(WallRow, WallColumn)){
		StopMovingWallAt(WallRow, WallColumn);
	}

	FTransform InstanceTransform;
	FVector NewLocation;
	Walls->GetInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform);
	NewLocation = InstanceTransform.GetLocation();
	NewLocation.Z = WallUnderGroundLevelZ;
	InstanceTransform.SetLocation(NewLocation);
	Walls->UpdateInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform, false, true);
}

void AWallManager::UpdateAllWallTransformsAtCurrentTime_Implementation() {
	FTransform InstanceTransform;
	float CurrentTime = GetWorld()->TimeSeconds;
	for (int32 WallIndex = 0; WallIndex < StartingLocations.Num();) {
		FVector NewLocation;
		int32 CurrentWallInstance = GetInstanceIndexForWallAt(MovingWallRowsAndColumns[WallIndex].y, MovingWallRowsAndColumns[WallIndex].x);
		float Alpha = (CurrentTime - WallMoveBeginTime[WallIndex]) / WallMovementIntervals[WallIndex];
		Alpha = Alpha > 1.f ? 1.f : Alpha;
		NewLocation = FMath::InterpEaseInOut(StartingLocations[WallIndex], TargetLocations[WallIndex], Alpha, 2.f);
		Walls->GetInstanceTransform(CurrentWallInstance, InstanceTransform);
		InstanceTransform.SetLocation(NewLocation);
		WallIndex + 1 == StartingLocations.Num() ? Walls->UpdateInstanceTransform(CurrentWallInstance, InstanceTransform, false, true) 
												: Walls->UpdateInstanceTransform(CurrentWallInstance, InstanceTransform);
		if (Alpha < 1.f) {
			WallIndex++;
		} else {
			StopMovingWallAt(MovingWallRowsAndColumns[WallIndex].y, MovingWallRowsAndColumns[WallIndex].x);
		}

	}
}

bool AWallManager::WallIsGroundLevelAt(int32 WallRow, int32 WallColumn){
	FTransform InstanceTransform;
	Walls->GetInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform);
	return FMath::IsNearlyEqual(InstanceTransform.GetLocation().Z, WallGroundLevelZ, .001f);
}

bool AWallManager::WallIsMidAirAt(int32 WallRow, int32 WallColumn){
	FTransform InstanceTransform;
	Walls->GetInstanceTransform(GetInstanceIndexForWallAt(WallRow, WallColumn), InstanceTransform);
	return FMath::IsNearlyEqual(InstanceTransform.GetLocation().Z, WallMidAirLevelZ, .001f);
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