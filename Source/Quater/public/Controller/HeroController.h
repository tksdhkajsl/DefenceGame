// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HeroController.generated.h"

/** Forward declaration to improve compiling times */
class UInputMappingContext;
class UInputAction;



UCLASS()
class AHeroController : public APlayerController
{
	GENERATED_BODY()

public:
	AHeroController();

	// 입력 매핑 컨텍스트 (IMC_Default)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// 스킬 입력 액션 (IA_Skill1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> Skill1Action;

	

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// 스킬 키 눌렀을 때 실행
	void OnSkill1Triggered();


};


