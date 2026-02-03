// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Data/GameDataTypes.h"
#include "BaseGameMode.generated.h"

/**
 * 
 */
UCLASS()
class QUATER_API ABaseGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ABaseGameMode();

	// --- [1] 인구수 제한  ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UnitCount")
	int32 MAX_ALLY_COUNT = 15; //아군

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UnitCount")
	int32 MAX_ENEMY_COUNT = 15; //적군

	//현재 유닛 수 확인
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameRule")
	int32 CurrentAllyCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameRule")
	int32 CurrentEnemyCount = 0;

	bool CanSpawnUnit(ETeamType Team);
	void RegisterUnit(ETeamType Team);
	void UnregisterUnit(ETeamType Team);

	
public:
	// --- [2] 승패 판정 로직 ---
	// 기지가 파괴되었을 때 호출되는 함수
	void OnGameOver(ETeamType DefeatedTeam);

	// 영웅 사망 시 호출될 함수
	void OnHeroDied(AController* HeroController);

protected:
	// 게임 종료 상태 플래그 (중복 호출 방지)
	bool bIsGameOver = false;

	// 리스폰 타이머 처리 함수
	void RespawnHero(AController* HeroController);

	// 리스폰 대기 시간 (초)
	UPROPERTY(EditDefaultsOnly, Category = "GameRule")
	float HeroRespawnDelay = 50.0f;

	// [BP 연동] 승리/패배 UI를 띄우기 위한 이벤트 함수
	// C++에서 호출하면 블루프린트(BP_QuaterGameMode)에서 UI를 CreateWidget 하면 됨
	UFUNCTION(BlueprintImplementableEvent, Category = "GameRule")
	void BP_ShowResultUI(bool bIsVictory);
};
