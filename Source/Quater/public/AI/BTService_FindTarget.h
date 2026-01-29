// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindTarget.generated.h"

/**
 * 
 */
UCLASS()
class QUATER_API UBTService_FindTarget : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_FindTarget();

protected:
	// 0.5초 등 주기마다 실행될 로직
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	float DetectRange = 800.0f; // 탐색 범위

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetKey; // 결과를 저장할 블랙보드 키 (TargetActor)
};
