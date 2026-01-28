// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h" // GAS 필수 인터페이스
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Data/GameDataTypes.h"
#include "BaseCharacter.generated.h"


// 델리게이트: 캐릭터 사망 시 GameMode나 풀링 시스템에 알림
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDiedDelegate, ABaseCharacter*, DeadCharacter);

UCLASS()
class QUATER_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

    // --- [1] GAS Components ---
        // VisibleAnywhere: 블루프린트에서 확인 가능, TObjectPtr: UE5 스마트 포인터
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    TObjectPtr<class UBaseAttributeSet> AttributeSet;

    // --- [2] 공통 전투 데이터 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    ETeamType TeamID; // Player, Ally, Enemy 구분

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FName CharacterID; // 데이터 테이블 조회용 ID (예: "Unit_Skeleton_01")

    // 사망 시그니처 (BP에서도 바인딩 가능)
    UPROPERTY(BlueprintAssignable, Category = "Combat")
    FOnCharacterDiedDelegate OnCharacterDied;

    // --- [3] 인터페이스 구현 ---
    virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
    virtual void BeginPlay() override;

    // --- [4] GAS 초기화 함수들 ---
    // 스탯 초기화 (체력, 마나, 공격력 등)
    virtual void InitializeAttributes();

    // 기본 스킬 부여 (평타 등)
    virtual void GiveStartupAbilities();

    // 초기 스탯 설정을 위한 GameplayEffect 클래스 (BP에서 지정)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
    TSubclassOf<class UGameplayEffect> DefaultStatsEffect;

    // 시작 시 주어질 스킬 목록
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
    TArray<TSubclassOf<class UGameplayAbility>> StartupAbilities;

public:
    // --- [5] 전투 및 상태 처리 ---
    // 사망 처리 (가상함수: 영웅은 게임오버, 유닛은 풀 반납)
    virtual void Die();

    // 체력 변경 콜백 (UI 업데이트용)
    virtual void OnHealthChanged(float NewHealth, float MaxHealth);

    // --- [6] 최적화 & 오브젝트 풀링 인터페이스 ---
    // 풀에서 꺼낼 때 호출 (초기화)
    virtual void ActivateUnit();

    // 풀에 넣기 전 호출 (정리)
    virtual void DeactivateUnit();

    // 현재 사망 상태인지 확인
    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool IsDead() const;

protected:
    // 사망 태그 (GAS 로직용)
    UPROPERTY(EditDefaultsOnly, Category = "GAS")
    FGameplayTag DeadTag;

};
