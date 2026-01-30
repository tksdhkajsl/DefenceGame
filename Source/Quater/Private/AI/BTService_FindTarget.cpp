// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_FindTarget.h"
#include "AIController.h"
#include "Character/BaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/BaseCharacter.h"
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
	ETeamType MyTeam = ETeamType::None;
	if (ABaseCharacter* MyChar = Cast<ABaseCharacter>(OwningPawn))
	{
		MyTeam = MyChar->TeamID;
	}

	// 2. 주변 검색 (APawn으로 검색해야 유닛+기지 모두 찾음)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwningPawn);

	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		OwningPawn->GetActorLocation(),
		DetectRange,
		ObjectTypes,
		APawn::StaticClass(), // BaseCharacter -> APawn으로 변경
		ActorsToIgnore,
		OutActors
	);

	AActor* NearestTarget = nullptr;
	float MinDistanceSq = FLT_MAX;

	for (AActor* Actor : OutActors)
	{
		// 3-1. 유닛(Character)인 경우 판별
		if (ABaseCharacter* TargetUnit = Cast<ABaseCharacter>(Actor))
		{
			// 적군이고, 살아있으면 타겟 후보
			if (TargetUnit->TeamID != MyTeam && !TargetUnit->IsDead())
			{
				float DistSq = FVector::DistSquared(TargetUnit->GetActorLocation(), OwningPawn->GetActorLocation());
				if (DistSq < MinDistanceSq)
				{
					MinDistanceSq = DistSq;
					NearestTarget = TargetUnit;
				}
			}
		}
		// 3-2. 기지(Structure)인 경우 판별
		else if (ABaseStructure* TargetBase = Cast<ABaseStructure>(Actor))
		{
			// 적군 기지라면 타겟 후보 (기지는 보통 죽으면 Collision이 꺼지므로 IsDead 체크 대신 유효성 체크)
			if (TargetBase->TeamID != MyTeam)
			{
				float DistSq = FVector::DistSquared(TargetBase->GetActorLocation(), OwningPawn->GetActorLocation());
				if (DistSq < MinDistanceSq)
				{
					MinDistanceSq = DistSq;
					NearestTarget = TargetBase;
				}
			}
		}
	}

	// 4. 블랙보드에 결과 저장 (없으면 null 저장됨 -> 추적 중지)
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, NearestTarget);
}
