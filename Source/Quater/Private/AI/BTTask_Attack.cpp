// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Attack.h"
#include "AIController.h"
#include "Character/BaseCharacter.h"
#include"AbilitySystemComponent.h"

UBTTask_Attack::UBTTask_Attack()
{
    NodeName = TEXT("GAS Attack");
    bNotifyTick = true; // 공격 끝나는지 감시하려고 Tick 켬
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Pawn);

	if (ASCInterface)
	{
		UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();

		// "Ability.Action.Attack" 태그를 가진 어빌리티 발동 시도
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Action.Attack")));

		if (ASC->TryActivateAbilitiesByTag(TagContainer))
		{
			return EBTNodeResult::InProgress; // 성공적으로 시작함 (끝날 때까지 대기)
		}
	}

	return EBTNodeResult::Failed; // 발동 실패 (마나 부족, 쿨타임 등)
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 공격 중인지 체크 로직 필요 (여기서는 간단히 태그 검사)
	// 실제로는 어빌리티가 끝났다는 델리게이트를 받거나, 태그가 빠졌는지 검사해서 FinishLatentTask 호출

	// 간단 구현: 해당 태그가 아직도 있으면 진행 중, 없으면 끝
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ABaseCharacter* Char = Cast<ABaseCharacter>(Pawn))
	{
		// 공격 태그가 빠졌으면(공격 모션 끝) 태스크 종료
		// 주의: 애니메이션 몽타주에서 Notify로 태그를 지워줘야 함
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
