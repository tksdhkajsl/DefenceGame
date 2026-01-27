// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BaseAttributeSet.generated.h"

//  매크로 사용
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class QUATER_API UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
    UBaseAttributeSet();

    // 네트워크 복제를 위한 필수 오버라이드
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // 속성 변경 전 처리 (Clamping 등)
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

    // GE(GameplayEffect) 실행 후 처리 (실제 데미지 적용, 사망 로직 등)
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

public:
    // --- [1] 생존 관련 속성 (Vital) ---

    // 현재 체력
    UPROPERTY(BlueprintReadOnly, Category = "Vital", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Health)

    // 최대 체력
    UPROPERTY(BlueprintReadOnly, Category = "Vital", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxHealth)

    // 현재 마나 (스킬 사용 자원)
    UPROPERTY(BlueprintReadOnly, Category = "Vital", ReplicatedUsing = OnRep_Mana)
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Mana)

    // 최대 마나
    UPROPERTY(BlueprintReadOnly, Category = "Vital", ReplicatedUsing = OnRep_MaxMana)
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxMana)

    // --- [2] 전투 관련 속성 (Combat) ---

    // 공격력 (장비 장착 시 변경됨)
    UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_AttackPower)
    FGameplayAttributeData AttackPower;
    ATTRIBUTE_ACCESSORS(UBaseAttributeSet, AttackPower)

    // 방어력 (데미지 감소용)
    UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_DefensePower)
    FGameplayAttributeData DefensePower;
    ATTRIBUTE_ACCESSORS(UBaseAttributeSet, DefensePower)

    // 이동 속도 (라인 디펜스에서 진군 속도 제어용)
    UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_MoveSpeed)
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MoveSpeed)

    // --- [3] 메타 속성 (Meta Attributes) ---
    // 네트워크로 복제되지 않으며, 서버에서 데미지 계산을 위한 '임시 변수'로 사용됨

    UPROPERTY(BlueprintReadOnly, Category = "Meta")
    FGameplayAttributeData Damage;
    ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Damage)

protected:
    // --- OnRep 함수들 (값이 바뀌면 클라이언트 UI 갱신용) ---
    UFUNCTION() virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
    UFUNCTION() virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
    UFUNCTION() virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);
    UFUNCTION() virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
    UFUNCTION() virtual void OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower);
    UFUNCTION() virtual void OnRep_DefensePower(const FGameplayAttributeData& OldDefensePower);
    UFUNCTION() virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);
};
