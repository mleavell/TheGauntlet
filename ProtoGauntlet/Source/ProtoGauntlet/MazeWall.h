// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MazeWall.generated.h"

UCLASS()
class PROTOGAUNTLET_API AMazeWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMazeWall();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable ,Category = "Raise and Lower")
	void Lower();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Raise and Lower")
	void LowerOnTimer(float LoweredTime);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Raise and Lower")
	void Raise();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Raise and Lower")
	void RaiseOnTimer(float RaisedTime);

	UPROPERTY(BlueprintReadWrite, Category = "Raise and Lower")
	bool LowerEnabled;

	UPROPERTY(BlueprintReadWrite, Category = "Raise and Lower")
	bool RaiseEnabled;

	
};
