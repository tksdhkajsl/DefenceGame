// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/BaseStructure.h"
#include "AbilitySystemComponent.h"
#include "Ability/BaseAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Mode/BaseGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseStructure::ABaseStructure()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	// 1. 충돌체 설정 (유닛이 밀고 들어오지 못하게 BlockAll)
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);
	CapsuleComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CapsuleComp->SetCapsuleHalfHeight(200.0f);
	CapsuleComp->SetCapsuleRadius(100.0f);

	// 2. 메쉬 설정
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	// 3. GAS 설정
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("AttributeSet"));

}

void ABaseStructure::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		// 초기 스탯 적용 (HP 등)
		if (InitStatEffect)
		{
			FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
			Context.AddSourceObject(this);
			AbilitySystemComponent->ApplyGameplayEffectToSelf(InitStatEffect.GetDefaultObject(), 1.0f, Context);
		}
	}
}

UAbilitySystemComponent* ABaseStructure::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABaseStructure::OnHealthChanged(float NewHealth, float MaxHealth)
{
	if (NewHealth <= 0.0f)
	{
		DestroyBase();
	}
}

void ABaseStructure::DestroyBase()
{
	// 1. 게임모드 가져오기
	ABaseGameMode* GM = Cast<ABaseGameMode>(UGameplayStatics::GetGameMode(this));
	if (GM)
	{
		// 2. 게임 종료 요청 (내 팀ID를 넘겨줌 -> 내가 졌다는 뜻)
		GM->OnGameOver(this->TeamID);
	}

	// 3. 기존 파괴 로직 (이펙트, 델리게이트 등)
	OnBaseDestroyed.Broadcast(TeamID);

	MeshComp->SetVisibility(false);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

