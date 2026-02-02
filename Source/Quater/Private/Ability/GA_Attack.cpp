// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_Attack.h"
#include "Character/BaseCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Mode/BaseGameplayTags.h"

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

	// 1. 자원(마나, 쿨타임) 확인 및 소모
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 2. 몽타주 재생 태스크 생성
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		AttackMontage // 에디터에서 지정한 몽타주
	);

	// 3. 몽타주 종료 처리 
	// K2_EndAbility는 델리게이트 시그니처가 안 맞을 수 있으므로, OnMontageEnded 같은 래퍼 함수를 쓰는 게 안전합니다.
	if (MontageTask)
	{
		MontageTask->OnBlendOut.AddDynamic(this, &UGA_Attack::OnMontageEnded);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_Attack::OnMontageEnded);
		MontageTask->OnCancelled.AddDynamic(this, &UGA_Attack::OnMontageEnded);
		MontageTask->OnCompleted.AddDynamic(this, &UGA_Attack::OnMontageEnded);
		MontageTask->ReadyForActivation();
	}

	// 4. 데미지 타이밍 기다리기 (Wait Gameplay Event)
	// 애니메이션 노티파이에서 'Event.Combat.Hit' 태그를 보내면 이 태스크가 반응함
	FGameplayTag HitTag = FBaseGameplayTags::Get().Event_Combat_Hit;

	UAbilityTask_WaitGameplayEvent* WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		HitTag,
		nullptr, // OptionalExternalTarget
		false,   // OnlyTriggerOnce (한 번만 맞게 할 건지, 여러 명 맞게 할 건지 - false면 지속 대기)
		false    // OnlyMatchExact
	);

	if (WaitTask)
	{
		// 이벤트가 오면 OnHitEventReceived 함수 실행
		WaitTask->EventReceived.AddDynamic(this, &UGA_Attack::OnHitEventReceived);
		WaitTask->ReadyForActivation();
	}
}

// 몽타주가 끝나면 어빌리티를 종료시키는 함수
void UGA_Attack::OnMontageEnded()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

// 실제 데미지를 주는 함수 (헤더에 선언 필요)
void UGA_Attack::OnHitEventReceived(FGameplayEventData Payload)
{
	// Payload.TargetData에 맞은 적의 정보가 들어옴 (Montage Notify에서 설정 필요)
	// 만약 Notify가 타겟 정보를 안 주면, 여기서 SphereOverlap 등으로 직접 찾아야 함.

	// 일단 간단하게 "내 주변 적"에게 데미지를 주는 방식 (광역 평타) 예시:
	/*
	if (DamageEffectClass)
	{
		// 여기서 OverlapCheck로 적을 찾아서 ApplyGameplayEffectToTarget 호출
	}
	*/
}
