// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "InputActionValue.h"
#include "HeroCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;

UCLASS()
class QUATER_API AHeroCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	AHeroCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// --- [1] 카메라 및 시점 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	// --- [2] 입력 액션 (WASD 이동용) ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Input")
	TObjectPtr<UInputAction> MoveAction;

	// --- [3] 자동 전투 설정 ---
	// True면 스킬도 알아서 사용, False면 스킬은 유저가 직접 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIsAutoSkillMode = false;

	// 자동 스킬 모드 토글 (UI 버튼 등에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ToggleAutoSkillMode();

protected:
	// --- [4] 내부 전투 로직 ---
	// 현재 타겟 (평타 칠 대상)
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	TObjectPtr<ABaseCharacter> CurrentTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRange = 700.0f;

	// 이동 함수 (WASD)
	void Move(const FInputActionValue& Value);

	// 주변 적 탐색
	void ScanForTarget();

	// 자동 공격 실행
	void PerformAutoBasicAttack(); // 평타 (무조건 자동)
	void PerformAutoSkill();       // 스킬 (모드에 따라)

};
