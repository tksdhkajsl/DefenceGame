// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include "Data/GameDataTypes.h"
#include "BaseStructure.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBaseDestroyed, ETeamType, DestroyedTeam);

UCLASS()
class QUATER_API ABaseStructure : public APawn, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	
	ABaseStructure();
	

protected:
	
	virtual void BeginPlay() override;



protected:

	// --- [1]기지 요소 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<class UCapsuleComponent> CapsuleComp; // 피격 판정용

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<class UStaticMeshComponent> MeshComp; // 건물 외형

	// --- [2] GAS (체력 관리용) ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UBaseAttributeSet> AttributeSet;

	// --- [3] 게임 로직 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	ETeamType TeamID; // Player(아군 기지) vs Enemy(적군 기지)

	UPROPERTY(BlueprintAssignable)
	FOnBaseDestroyed OnBaseDestroyed; // 파괴 시 게임모드에 알림

	// GAS 초기화
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> InitStatEffect;

public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 체력 변경 콜백 (AttributeSet에서 호출)
	void OnHealthChanged(float NewHealth, float MaxHealth);

	// 파괴 처리
	void DestroyBase();
};
