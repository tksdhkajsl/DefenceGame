// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "Ability/BaseAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pawn/BaseStructure.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	PrimaryActorTick.bCanEverTick = false; // 최적화: 기본적으로 Tick 끔 (필요한 경우만 켬)

    // 1. GAS 컴포넌트 생성
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    // 최적화: 유닛이 많으므로 'Minimal' 모드 권장 (영웅만 Mixed로 변경 고려)
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    // 2. AttributeSet 생성
    AttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("AttributeSet"));

    // 3. 충돌 및 이동 설정 (RVO 회피 준비)
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
    GetCharacterMovement()->bUseRVOAvoidance = true; // RVO 활성화
    GetCharacterMovement()->AvoidanceWeight = 0.5f;

}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
    // 주의: 여기서 InitAbilityActorInfo를 부르지 않음.
    // 영웅은 PossessedBy, 유닛은 Spawn/Activate 시점에 별도로 호출함.
	
}

void ABaseCharacter::InitializeAttributes()
{
    if (!AbilitySystemComponent || !DefaultStatsEffect) return;

    // GameplayEffect를 통해 초기 스탯(HP, Attack 등) 적용
    FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
    ContextHandle.AddSourceObject(this);
    FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultStatsEffect, 1.0f, ContextHandle);

    if (SpecHandle.IsValid())
    {
        AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
}

void ABaseCharacter::GiveStartupAbilities()
{
    if (!AbilitySystemComponent || GetLocalRole() != ROLE_Authority) return;

    // 시작 스킬 부여 (서버만 수행)
    for (TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
    {
        FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
        AbilitySystemComponent->GiveAbility(Spec);
    }
}

void ABaseCharacter::Die()
{
    // 이미 죽었으면 리턴
    if (IsDead()) return;

    // 1. 사망 태그 부착 (다른 행동 불가하게 막음)
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
    }

    // 2. 콜리전 끄기 (시체 길막 방지)
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 3. 델리게이트 (GameMode나 PoolManager가 처리)
    OnCharacterDied.Broadcast(this);

    // 4. 래그돌 처리 (선택)
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
    SetLifeSpan(5.0f);
}

bool ABaseCharacter::IsDead() const
{
    if (AbilitySystemComponent)
    {
        return AbilitySystemComponent->HasMatchingGameplayTag(DeadTag);
    }
    return false;
}

void ABaseCharacter::OnHealthChanged(float NewHealth, float MaxHealth)
{
    // 자식 클래스(Hero/Unit)에서 UI 업데이트 로직 구현
    if (NewHealth <= 0.0f)
    {
        Die();
    }
}



void ABaseCharacter::ActivateUnit()
{
    // 풀링: 재사용될 때 상태 리셋
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

    GetMesh()->SetSimulatePhysics(false);
    GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

    // GAS 태그 초기화 (죽음 태그 제거 등)
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->RemoveLooseGameplayTag(DeadTag);
    }

    // 스탯 리셋 (HP 100%로)
    InitializeAttributes();
}

void ABaseCharacter::DeactivateUnit()
{
    // 풀링: 반납 전 정리 작업
    // 필요 시 타이머 초기화, 바인딩 해제 등
}

