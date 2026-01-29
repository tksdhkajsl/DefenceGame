// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Attack.generated.h"

/**
 * 기본 평타 공격 어빌리티
 */

UCLASS()
class QUATER_API UGA_Attack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	UGA_Attack();

	//어빌리티 발동 시 실행되는 함수
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	//공격 애니메이션
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	//데미지 효과(GE_Damage)
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;

	//공격 태그(쿨타임 등의 용도)
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FGameplayTag AttackTag;

	//몽타주 종료 시 호출될 콜백 함수
	UFUNCTION()
	void OnMontageEnded();
};
