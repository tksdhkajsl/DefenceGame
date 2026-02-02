// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "UnitData.generated.h"

/**
 * 유닛의 '스탯'과 '외형'을 정의하는 데이터 에셋
 * 우클릭 -> Miscellaneous -> Data Asset -> UnitData 로 생성 가능
 */

UCLASS()
class QUATER_API UUnitData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// --- [1] 기본 정보 ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
	FText UnitName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
	UTexture2D* Icon;

	// 유닛의 속성 (예: 불, 물, 풀 - 상성 계산용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
	FGameplayTag UnitTypeTag;

	// --- [2] 외형 ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	TSubclassOf<UAnimInstance> AnimBlueprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	UAnimMontage* AttackMontage;

	// --- [3] 능력치 (GAS 초기화용) ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float AttackDamage = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float AttackRange = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MoveSpeed = 300.0f;

	// 투사체 클래스 (원거리 유닛인 경우)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<class AProjectileActor> ProjectileClass;
};
