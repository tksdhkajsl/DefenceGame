// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UnitSpawner.h"
#include "Character/UnitCharacter.h"
#include "Mode/BaseGameMode.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AUnitSpawner::AUnitSpawner()
{
 	
	PrimaryActorTick.bCanEverTick = false;

}


void AUnitSpawner::BeginPlay()
{
	Super::BeginPlay();

	// 적군 기지라면 자동으로 주기적 소환 시작
	if (SpawnInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AUnitSpawner::SpawnUnit, SpawnInterval, true);
	}
}

void AUnitSpawner::SpawnUnit()
{
	// 1. 유닛 클래스 할당 여부 체크
	if (!UnitClassToSpawn) return;

	// 2. 게임모드 가져오기
	ABaseGameMode* GM = Cast<ABaseGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GM) return;

	// 3. [핵심] 인구수 제한 체크 (꽉 찼으면 스폰 취소)
	if (!GM->CanSpawnUnit(SpawnTeamID))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Spawn Failed: Population Full!"));
		return;
	}

	// 4. 유닛 생성 (SpawnActor)
	// 나중에 최적화를 위해 이 부분을 'Object Pooling'으로 교체 가능
	FVector SpawnLoc = GetActorLocation();
	FRotator SpawnRot = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AUnitCharacter* NewUnit = GetWorld()->SpawnActor<AUnitCharacter>(UnitClassToSpawn, SpawnLoc, SpawnRot, SpawnParams);

	if (NewUnit)
	{
		// 5. 유닛 초기화
		NewUnit->TeamID = SpawnTeamID; // 팀 설정
		NewUnit->ActivateUnit();       // GAS 초기화 및 AI 가동 (필수!)

		// 6. [핵심] 게임모드에 등록 (인구수 카운트 +1)
		GM->RegisterUnit(SpawnTeamID);
	}
}



