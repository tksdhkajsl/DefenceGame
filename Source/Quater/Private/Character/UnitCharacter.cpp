// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UnitCharacter.h"
#include "Ability/BaseAttributeSet.h"
#include "Controller/MainAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AUnitCharacter::AUnitCharacter()
{
    // 스폰되거나 맵에 배치되면 자동으로 AI 컨트롤러 생성
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // 이동 컴포넌트 설정 (RVO 회피)
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->bUseRVOAvoidance = true;
        GetCharacterMovement()->AvoidanceWeight = 0.5f;
        GetCharacterMovement()->bOrientRotationToMovement = true;
    }

    // AI는 카메라 회전 불필요
    bUseControllerRotationYaw = false;
}

void AUnitCharacter::BeginPlay()
{

}

void AUnitCharacter::ActivateUnit()
{
    // 1. 부모 클래스 로직 (HP 리셋, 물리 켜기, 사망 태그 제거)
    Super::ActivateUnit();

    // 2. GAS 초기화 (유닛은 Owner와 Avatar가 모두 자신)
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);

        // 중복 부여 방지를 위해 기존 효과 정리 후 다시 부여
        AbilitySystemComponent->CancelAllAbilities();
        InitializeAttributes();   // 스탯 리셋
        GiveStartupAbilities();   // 스킬 다시 부여
    }

    // 3. AI 재가동
    // GetController()는 AController*를 반환하므로 캐스팅 필요
    AMainAIController* AICon = Cast<AMainAIController>(GetController());

    if (AICon && DefaultBehaviorTree)
    {
        // 블랙보드 초기화 및 트리 실행
        if (DefaultBehaviorTree->BlackboardAsset)
        {
            AICon->GetBlackboardComponent()->InitializeBlackboard(*DefaultBehaviorTree->BlackboardAsset);
        }
        AICon->RunBehaviorTree(DefaultBehaviorTree);
    }
}

void AUnitCharacter::DeactivateUnit()
{
    // 1. AI 정지
    AMainAIController* AICon = Cast<AMainAIController>(GetController());
    if (AICon)
    {
        AICon->StopMovement();

        // BrainComponent가 있으면 로직 중단
        if (UBrainComponent* Brain = AICon->GetBrainComponent())
        {
            Brain->StopLogic("Unit Returned to Pool");
        }
    }

    // 2. GAS 정리
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->CancelAllAbilities();
    }

    // 3. 부모 클래스 정리 (콜리전 끄기 등)
    Super::DeactivateUnit();
}

