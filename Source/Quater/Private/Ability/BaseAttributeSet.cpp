// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/BaseAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/BaseCharacter.h"

UBaseAttributeSet::UBaseAttributeSet()
{
    // 기본값 초기화 (필요 시)
}

void UBaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // DOREPLIFETIME_CONDITION_NOTIFY: 값이 바뀔 때만 복제하며, OnRep 함수를 호출함
    DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Mana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, DefensePower, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetHealthAttribute())
    {
        // 체력은 0 ~ MaxHealth 사이여야 함
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    else if (Attribute == GetManaAttribute())
    {
        // 마나도 0 ~ MaxMana 사이
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
    }
}

void UBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
    UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
    AActor* SourceActor = Source ? Source->GetAvatarActor() : nullptr;
    AActor* TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();

    // 1. 데미지 처리 (Meta Attribute 'Damage'가 들어왔을 때)
    if (Data.EvaluatedData.Attribute == GetDamageAttribute())
    {
        const float LocalDamage = GetDamage();
        SetDamage(0.0f); // 메타 속성은 사용 후 바로 초기화

        if (LocalDamage > 0.0f)
        {
            // 실제 체력 깎기
            const float NewHealth = GetHealth() - LocalDamage;
            SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));

            // 사망 체크
            if (GetHealth() <= 0.0f)
            {
                ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(TargetActor);
                if (TargetCharacter)
                {
                    TargetCharacter->Die(); // 사망 함수 호출
                }
            }

            // (선택) UI용 데미지 텍스트 띄우기 요청 (Broadcast)
            // 100마리 단위 게임에서는 액터 스폰 대신 가벼운 UI 이벤트를 권장
        }
    }
    // 2. 이동 속도 변경 처리 (슬로우, 헤이스트 버프 등)
    else if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
    {
        ACharacter* TargetChar = Cast<ACharacter>(TargetActor);
        if (TargetChar && TargetChar->GetCharacterMovement())
        {
            TargetChar->GetCharacterMovement()->MaxWalkSpeed = GetMoveSpeed();
        }
    }
}

// --- OnRep 구현부 (클라이언트 동기화) ---
// GAS 내부 매크로를 사용하여 이전 값과 비교 후 동기화 처리
void UBaseAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Health, OldHealth);
    // UI 업데이트 함수 호출 (예: 체력바 갱신)
}

void UBaseAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxHealth, OldMaxHealth);
}

void UBaseAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Mana, OldMana);
}

void UBaseAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxMana, OldMaxMana);
}

void UBaseAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, AttackPower, OldAttackPower);
}

void UBaseAttributeSet::OnRep_DefensePower(const FGameplayAttributeData& OldDefensePower)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, DefensePower, OldDefensePower);
}

void UBaseAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MoveSpeed, OldMoveSpeed);
}
