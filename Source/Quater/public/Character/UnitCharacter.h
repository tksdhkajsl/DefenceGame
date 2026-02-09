// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "UnitCharacter.generated.h"

/**
 * 아군 소환수 및 적군 몬스터 전용 클래스
 * 특징: PlayerState가 없으며, AIController에 의해 제어됨.
 */
class UBehaviorTree;
class UUnitData;

UCLASS()
class QUATER_API AUnitCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AUnitCharacter();	
	// --- 1. 데이터 에셋  ---
	// 스포너가 이 변수에 데이터를 넣어주면, 유닛이 그 모습으로 변신합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TObjectPtr<UUnitData> UnitData;

	// --- 2. AI 설정 ---
	// 이 유닛이 스폰되자마자 실행할 행동 트리 (예: 근거리용 BT, 원거리용 BT)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UBehaviorTree> DefaultBehaviorTree;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	AActor* GetTargetEnemy(); // 블랙보드에서 타겟을 가져오는 함수

	// --- 3. 오브젝트 풀링 (BaseCharacter 오버라이드) ---
	// 풀에서 꺼내질 때: GAS 초기화 + AI 재가동
	virtual void ActivateUnit() override;

	// 풀로 돌아갈 때: AI 중지 + GAS 정리
	virtual void DeactivateUnit() override;

	// 사망 처리 함수 (C++에서 호출)
	virtual void OnDeath();

	// 사망 시 블루프린트에서 연출(랙돌 등)을 하기 위한 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void BP_OnDeath();

protected:
	// 스폰 시 AI 컨트롤러 자동 빙의 설정
	virtual void BeginPlay() override;

	// 데이터 에셋 내용을 실제 캐릭터에 적용하는 함수
	virtual void InitUnitByData();

	// 중복 사망 방지용 플래그
	bool bIsDead = false;
};
