// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_FindTarget.h"
#include "AIController.h"
#include "Character/BaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Pawn/BaseStructure.h"
#include "Character/HeroCharacter.h" // 영웅 구분용
#include "Pawn/BaseStructure.h"
#include "Data/GameDataTypes.h"

UBTService_FindTarget::UBTService_FindTarget()
{
	NodeName = TEXT("Find Nearest Target");
	Interval = 0.5f; // 0.5초마다 탐색
}

void UBTService_FindTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 1. 내 AI Pawn 가져오기
	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!OwningPawn) return;

	// 내 팀 정보 가져오기 (UnitCharacter라고 가정)
	ABaseCharacter* MyChar = Cast<ABaseCharacter>(OwningPawn);
	if (!MyChar) return;

	ETeamType MyTeam = MyChar->TeamID;

	// 2. 주변 모든 Pawn 검색
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(OwningPawn);
	TArray<AActor*> OutActors;

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), OwningPawn->GetActorLocation(), DetectRange, ObjectTypes, APawn::StaticClass(), IgnoreActors, OutActors);

	// 3. 우선순위별 후보군 분류 (거리 비교용)
	AActor* Priority1 = nullptr; // 1순위
	AActor* Priority2 = nullptr; // 2순위
	AActor* Priority3 = nullptr; // 3순위

	float DistSq_Prio1 = FLT_MAX;
	float DistSq_Prio2 = FLT_MAX;
	float DistSq_Prio3 = FLT_MAX;

	for (AActor* Actor : OutActors)
	{
		float DistSq = FVector::DistSquared(Actor->GetActorLocation(), OwningPawn->GetActorLocation());

		// A. 유닛 또는 영웅인 경우
		if (ABaseCharacter* TargetChar = Cast<ABaseCharacter>(Actor))
		{
			if (TargetChar->IsDead()) continue; // 죽은 적 무시

			// [아군 AI] 로직
			if (MyTeam == ETeamType::Ally)
			{
				// 1순위: 적 유닛 (Enemy)
				if (TargetChar->TeamID == ETeamType::Enemy)
				{
					if (DistSq < DistSq_Prio1) { DistSq_Prio1 = DistSq; Priority1 = TargetChar; }
				}
			}
			// [적군 AI] 로직
			else if (MyTeam == ETeamType::Enemy)
			{
				// 1순위: 아군 유닛 (Ally)
				if (TargetChar->TeamID == ETeamType::Ally)
				{
					if (DistSq < DistSq_Prio1) { DistSq_Prio1 = DistSq; Priority1 = TargetChar; }
				}
				// 2순위: 플레이어 영웅 (Player)
				else if (TargetChar->TeamID == ETeamType::Player)
				{
					if (DistSq < DistSq_Prio2) { DistSq_Prio2 = DistSq; Priority2 = TargetChar; }
				}
			}
		}
		// B. 기지(Structure)인 경우
		else if (ABaseStructure* TargetBase = Cast<ABaseStructure>(Actor))
		{
			// [아군 AI] 2순위: 적 기지
			if (MyTeam == ETeamType::Ally && TargetBase->TeamID == ETeamType::Enemy)
			{
				if (DistSq < DistSq_Prio2) { DistSq_Prio2 = DistSq; Priority2 = TargetBase; }
			}
			// [적군 AI] 3순위: 아군(플레이어) 기지
			else if (MyTeam == ETeamType::Enemy && TargetBase->TeamID == ETeamType::Player)
			{
				if (DistSq < DistSq_Prio3) { DistSq_Prio3 = DistSq; Priority3 = TargetBase; }
			}
		}
	}

	// 3. 최종 타겟 선정 (1순위 -> 2순위 -> 3순위 순서로 체크)
	AActor* FinalTarget = nullptr;
	if (Priority1) FinalTarget = Priority1;
	else if (Priority2) FinalTarget = Priority2;
	else if (Priority3) FinalTarget = Priority3;

	// 4. 블랙보드 업데이트
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, FinalTarget);
}
