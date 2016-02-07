// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MazeWall.h"
#include "MyActor.generated.h"

namespace PGUtilities
{
	void printString(FString InputString);
}

UENUM(BlueprintType)
enum class EDirection : uint8
{
	D_North		UMETA(DisplayName = "North"),
	D_East		UMETA(DisplayName = "East"),
	D_South		UMETA(DisplayName = "South"),
	D_West		UMETA(DisplayName = "West"),
	D_Up		UMETA(DisplayName = "Up"),
	D_Down		UMETA(DisplayName = "Down"),
	D_None		UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class ETileDesignation : uint8
{
	TD_Wall		UMETA(DisplayName = "Wall"),
	TD_Cell		UMETA(DisplayName = "Cell"),
	TD_Path		UMETA(DisplayName = "Path"),
	TD_Visited		UMETA(DisplayName = "Visited"),
	TD_OutOfBounds		UMETA(DisplayName = "Out of Bounds")
};

USTRUCT(BlueprintType)
struct FMazeRowData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze Row Data")
	TArray<ETileDesignation> Column;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze Row Data")
	TArray<AMazeWall*> ColumnWallRef;
};

USTRUCT(BlueprintType)
struct FIntPair
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Int Pair")
	int32 x;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Int Pair")
	int32 y;

	FIntPair()
	{
		x = 0;
		y = 0;
	}

	FIntPair(int32 InputX, int32 InputY)
	{
		x = InputX;
		y = InputY;
	}

	bool operator ==(const FIntPair & OtherPair) const {
		return x == OtherPair.x || y == OtherPair.y;
	}

	bool operator !=(FIntPair & OtherPair) {
		return x != OtherPair.x || y != OtherPair.y;
	}
};

UCLASS()
class PROTOGAUNTLET_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
