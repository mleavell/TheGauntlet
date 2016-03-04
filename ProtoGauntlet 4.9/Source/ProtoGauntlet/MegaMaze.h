// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MegaMaze.generated.h"

UCLASS()
class PROTOGAUNTLET_API AMegaMaze : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, Category = "Dimensions")
	TSubclassOf<class AMazeSegment> MazeSegmentClass;

	/** Only odd values. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	int32 WidthInMazeSegments;

	/** Only odd values. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	int32 HeightInMazeSegments;

	/** Only odd values. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	int32 MazeLengthInTiles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	float TileSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	float FloorHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	float InnerWallHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	float OuterWallHeight;

	// Sets default values for this actor's properties
	AMegaMaze();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);

	private:

	void CalculateValues();
	
	
};
