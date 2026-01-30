// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/MainAIController.h"
#include "Character/UnitCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h" // RVO 설정용

AMainAIController::AMainAIController()
{
    // 1. 블랙보드 컴포넌트 생성
    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

    // 2. RVO(군중 회피) 설정
    // 일반 AIController는 이 컴포넌트가 없어서 유닛끼리 겹침
    // DetourCrowdAIController를 상속받으면 자동으로 설정되지만, 세부 튜닝 가능
    // UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent());
    // if (CrowdComp)
    // {
    //     CrowdComp->SetCrowdSimulationState(ECrowdSimulationState::Enabled);
    // }
}

void AMainAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // 1. 빙의한 폰이 우리 유닛인지 확인
    AUnitCharacter* Unit = Cast<AUnitCharacter>(InPawn);
    if (Unit)
    {
        // 2. 팀 ID 설정 (아군: 0, 적군: 1 등으로 매핑)
        // Unit->TeamID는 우리가 만든 Enum이고, FGenericTeamId는 엔진 표준 타입
        CachedTeamId = FGenericTeamId((uint8)Unit->TeamID);

        // 3. 비헤이비어 트리 실행
        if (Unit->DefaultBehaviorTree)
        {
            // 블랙보드 초기화 (키 값들이 유효한지 확인)
            if (Unit->DefaultBehaviorTree->BlackboardAsset)
            {
                BlackboardComp->InitializeBlackboard(*Unit->DefaultBehaviorTree->BlackboardAsset);
            }

            // 트리 실행
            RunBehaviorTree(Unit->DefaultBehaviorTree);
        }
    }
}

void AMainAIController::OnUnPossess()
{
    // 풀링 시스템을 위해 행동 중지
    if (BlackboardComp)
    {
        // ▼▼▼ [수정] 이 줄을 삭제하세요. (존재하지 않는 함수) ▼▼▼
        // AI검수 결과 불필요 라는데. 혹시몰라 남겨둠. 풀링 시스템 꼬임 방지 방어 코드
        // BlackboardComp->ClearAllKeyValues(); 
    }

    StopMovement();

    // 행동 트리 로직 정지 ("Pool"은 로그용 이유 태그)
    if (BrainComponent)
    {
        BrainComponent->StopLogic("Pool");
    }

    Super::OnUnPossess();
}

FGenericTeamId AMainAIController::GetGenericTeamId() const
{
    return CachedTeamId;
}
