// Fill out your copyright notice in the Description page of Project Settings.


#include "Mode/BaseGameplayTags.h"
#include "GameplayTagsManager.h"

FBaseGameplayTags FBaseGameplayTags::GameplayTags;

void FBaseGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	// --- 태그 등록 (변수 = 태그 이름) ---

	GameplayTags.Ability_Action_Attack = Manager.AddNativeGameplayTag(
		FName("Ability.Action.Attack"),
		TEXT("기본 공격(무조건 자동)")
	);

	GameplayTags.Ability_Action_Skill1 = Manager.AddNativeGameplayTag(
		FName("Ability.Action.Skill1"),
		TEXT("Skill Slot 1")
	);

	GameplayTags.Event_Combat_Hit = Manager.AddNativeGameplayTag(
		FName("Event.Combat.Hit"),
		TEXT("Animation Hit Event")
	);

	GameplayTags.State_Dead = Manager.AddNativeGameplayTag(
		FName("State.Dead"),
		TEXT("사망 판정")
	);
}
