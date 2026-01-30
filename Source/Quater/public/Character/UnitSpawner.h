// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/GameDataTypes.h"
#include "Character/UnitCharacter.h"
#include "UnitSpawner.generated.h"

UCLASS()
class QUATER_API AUnitSpawner : public AActor
{
	GENERATED_BODY()

public:
	
	AUnitSpawner();

protected:
	
	virtual void BeginPlay() override;

public:
	// --- [1] 스폰 설정 ---
	// 생성할 유닛 클래스
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AUnitCharacter> UnitClassToSpawn;

	// 생성 주기 (0이면 자동 생성 안 함 -> 플레이어 기지용)
	UPROPERTY(EditAnywhere, Category = "Spawn")
	float SpawnInterval = 5.0f;

	// 누구 팀 유닛인가?
	UPROPERTY(EditAnywhere, Category = "Spawn")
	ETeamType SpawnTeamID;

	// --- [2] 스폰 로직 ---
	// 실제 유닛 생성 함수
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SpawnUnit();

private:
	FTimerHandle SpawnTimerHandle;



};
