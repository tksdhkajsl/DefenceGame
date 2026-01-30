// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_Attack.h"
#include "Character/BaseCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_Attack::UGA_Attack()
{
	// (InstancedPerActor)으로 설정해야 몽타주 태스크가 안전함
	// 검색 결과 게임 최적화 기능 향상에 많이 사용된다고 확인.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 몽타주 재생 태스크 생성
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		AttackMontage
	);

	//콜백 함수 연결
	//OnMontageEnded를 연결해야 함
	if (Task)
	{
		Task->OnBlendOut.AddDynamic(this, &UGA_Attack::OnMontageEnded);
		Task->OnInterrupted.AddDynamic(this, &UGA_Attack::OnMontageEnded);
		Task->OnCancelled.AddDynamic(this, &UGA_Attack::OnMontageEnded);
		Task->OnCompleted.AddDynamic(this, &UGA_Attack::OnMontageEnded);

		Task->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

// 몽타주가 끝나면 어빌리티를 종료시키는 함수
void UGA_Attack::OnMontageEnded()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
