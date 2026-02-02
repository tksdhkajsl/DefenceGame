// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

struct FBaseGameplayTags
{
public:
	static const FBaseGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	// --- 태그 변수 선언 ---
	FGameplayTag Ability_Action_Attack;
	FGameplayTag Ability_Action_Skill1;
	FGameplayTag Event_Combat_Hit;
	FGameplayTag State_Dead;
	// 필요한 만큼 추가...

protected:
	// 싱글톤 인스턴스
	static FBaseGameplayTags GameplayTags;
};