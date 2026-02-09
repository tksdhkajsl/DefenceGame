// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UnitCharacter.h"
#include "Ability/BaseAttributeSet.h"
#include "Controller/MainAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Data/UnitData.h"

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
    Super::BeginPlay();

    //이 코드가 반드시 있어야 몽타주가 실행
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }

    // 배치된 유닛의 경우 BeginPlay에서도 초기화 시도
    if (UnitData)
    {
        InitUnitByData();
    }
}


void AUnitCharacter::ActivateUnit()
{
    // 1. 데이터 기반 초기화 (가장 먼저 실행)
    if (UnitData)
    {
        InitUnitByData();
    }

    // 2. 부모 클래스 로직 (HP 리셋 등)
    Super::ActivateUnit();

    // 3. GAS 초기화
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);

        // 기존 효과 정리 후 스탯/스킬 다시 부여
        AbilitySystemComponent->CancelAllAbilities();
        InitializeAttributes();
        GiveStartupAbilities();
    }

    // 4. AI 재가동
    AMainAIController* AICon = Cast<AMainAIController>(GetController());
    if (AICon && DefaultBehaviorTree)
    {
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


void AUnitCharacter::InitUnitByData()
{
    if (!UnitData) return;

    // 1. 외형 변경 (메쉬)
    if (UnitData->SkeletalMesh)
    {
        GetMesh()->SetSkeletalMesh(UnitData->SkeletalMesh);
    }

    // 2. 애니메이션 변경
    if (UnitData->AnimBlueprint)
    {
        GetMesh()->SetAnimInstanceClass(UnitData->AnimBlueprint);
    }

    // 3. 이동 속도 설정
    GetCharacterMovement()->MaxWalkSpeed = UnitData->MoveSpeed;

    // 4. 공격 사거리 설정 (AI 감지 범위 등으로 활용 가능)
    // AttackRange = UnitData->AttackRange; (BaseCharacter에 변수가 있다면)

    // 5. 투사체 클래스 설정 (원거리/근거리 구분용)
    // BaseCharacter.h에 추가한 ProjectileClass 변수에 값을 넣어줌
    this->ProjectileClass = UnitData->ProjectileClass;

    // 6. 스탯 적용 (AttributeSet 값 덮어쓰기)
    if (AttributeSet)
    {
        // Init 수동 설정 (또는 GE_InitStats를 동적으로 생성해서 적용해도 됨)
        AttributeSet->InitMaxHealth(UnitData->MaxHealth);
        AttributeSet->InitHealth(UnitData->MaxHealth);
        AttributeSet->InitAttackPower(UnitData->AttackDamage);
    }
}

AActor* AUnitCharacter::GetTargetEnemy()
{
    if (AAIController* AICon = Cast<AAIController>(GetController()))
    {
        if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
        {
            return Cast<AActor>(BB->GetValueAsObject("TargetActor")); // 블랙보드 키 이름
        }
    }
    return nullptr;
}