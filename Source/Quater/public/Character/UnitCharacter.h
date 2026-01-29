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

UCLASS()
class QUATER_API AUnitCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AUnitCharacter();	

	// --- [1] AI 설정 ---
	// 이 유닛이 스폰되자마자 실행할 행동 트리 (예: 근거리용 BT, 원거리용 BT)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UBehaviorTree> DefaultBehaviorTree;

	// --- [2] 오브젝트 풀링 (BaseCharacter 오버라이드) ---
	// 풀에서 꺼내질 때: GAS 초기화 + AI 재가동
	virtual void ActivateUnit() override;

	// 풀로 돌아갈 때: AI 중지 + GAS 정리
	virtual void DeactivateUnit() override;

protected:
	// 스폰 시 AI 컨트롤러 자동 빙의 설정
	virtual void BeginPlay() override;
};
