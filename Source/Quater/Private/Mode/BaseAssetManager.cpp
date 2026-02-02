// Fill out your copyright notice in the Description page of Project Settings.


#include "Mode/BaseAssetManager.h"
#include "Mode/BaseGameplayTags.h"


UBaseAssetManager& UBaseAssetManager::Get()
{
	check(GEngine);

	
	UBaseAssetManager* MyAssetManager = Cast<UBaseAssetManager>(GEngine->AssetManager);
	return *MyAssetManager;
}

void UBaseAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// 태그 초기화 함수 호출 (이게 없으면 태그가 등록 안 됨!)
	FBaseGameplayTags::InitializeNativeTags();
	
	// 로그로 확인
	//UE_LOG(LogTemp, Warning, TEXT("Game Tags Initialized!"));
}
