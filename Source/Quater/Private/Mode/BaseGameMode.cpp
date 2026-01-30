// Fill out your copyright notice in the Description page of Project Settings.


#include "Mode/BaseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

ABaseGameMode::ABaseGameMode()
{
	// 인구수 초기화
	CurrentAllyCount = 0;
	CurrentEnemyCount = 0;
	bIsGameOver = false;
}

// --- [1] 인구수 제한 로직 ---
bool ABaseGameMode::CanSpawnUnit(ETeamType Team)
{
	if (Team == ETeamType::Ally)
	{
		// 아군 유닛이 15마리 미만일 때만 스폰 가능
		return CurrentAllyCount < MAX_ALLY_COUNT;
	}
	else if (Team == ETeamType::Enemy)
	{
		// 적군 유닛이 15마리 미만일 때만 스폰 가능
		return CurrentEnemyCount < MAX_ENEMY_COUNT;
	}

	// 플레이어나 중립 유닛은 제한 없음
	return true;
}

void ABaseGameMode::RegisterUnit(ETeamType Team)
{
	if (Team == ETeamType::Ally)
	{
		CurrentAllyCount++;
	}
	else if (Team == ETeamType::Enemy)
	{
		CurrentEnemyCount++;
	}

	// (디버깅용 로그) 현재 인구수 확인
	// UE_LOG(LogTemp, Log, TEXT("Current Population - Ally: %d, Enemy: %d"), CurrentAllyCount, CurrentEnemyCount);
}

void ABaseGameMode::UnregisterUnit(ETeamType Team)
{
	if (Team == ETeamType::Ally)
	{
		CurrentAllyCount = FMath::Max(0, CurrentAllyCount - 1);
	}
	else if (Team == ETeamType::Enemy)
	{
		CurrentEnemyCount = FMath::Max(0, CurrentEnemyCount - 1);
	}
}

// --- [2] 승패 판정 로직 ---
void ABaseGameMode::OnGameOver(ETeamType DefeatedTeam)
{
	// 1. 중복 호출 방지 (이미 게임이 끝났으면 무시)
	if (bIsGameOver) return;
	bIsGameOver = true;

	// 2. 승리 여부 판단
	// 파괴된 기지가 적군기지 -> 플레이어 승리 (true)
	// 파괴된 기지가 아군기지 -> 플레이어 패배 (false)
	bool bIsVictory = (DefeatedTeam == ETeamType::Enemy);

	UE_LOG(LogTemp, Warning, TEXT("Game Over! Player Won: %s"), bIsVictory ? TEXT("True") : TEXT("False"));

	// 3. 결과 UI 호출 (블루프린트 이벤트 실행)
	BP_ShowResultUI(bIsVictory);

	// 4. 연출: 슬로우 모션 (0.1배속)
	UGameplayStatics::SetGlobalTimeDilation(this, 0.1f);

	// 5. 플레이어 조작 차단
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		// 입력, 이동, 회전 모두 막음
		PC->SetCinematicMode(true, false, false, true, true);
		PC->bShowMouseCursor = true; // 결과창 클릭을 위해 커서는 보이게
	}
}
