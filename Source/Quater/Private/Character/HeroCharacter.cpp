// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/HeroCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Mode/BaseGameMode.h"
#include "GameplayTagContainer.h"

AHeroCharacter::AHeroCharacter()
{
	// Tick 활성화 (자동 전투 검색용)
	PrimaryActorTick.bCanEverTick = true;

	// 1. 캐릭터 회전 설정 (이동 방향으로 회전)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true; // 평면 이동 고정
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// 2. 카메라 붐 (탑뷰 설정)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // 캐릭터가 돌 때 카메라는 안 돔
	CameraBoom->TargetArmLength = 1200.0f;      // 거리
	CameraBoom->SetRelativeRotation(FRotator(-50.0f, 0.0f, 0.0f)); // 쿼터뷰 각도
	CameraBoom->bDoCollisionTest = false;       // 장애물 무시

	// 3. 카메라
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void AHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 이동(WASD) 바인딩
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHeroCharacter::Move);
		}
	}
}

void AHeroCharacter::Move(const FInputActionValue& Value)
{
	// 입력값 (X: 좌우, Y: 상하)
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 카메라가 바라보는 방향 기준이 아닌, 절대 월드 좌표 기준 이동 (탑뷰에 적합)
		// 만약 카메라 회전과 맞추려면 GetControlRotation() 사용

		// 전후 이동 (World Forward)
		AddMovementInput(FVector::ForwardVector, MovementVector.Y);

		// 좌우 이동 (World Right)
		AddMovementInput(FVector::RightVector, MovementVector.X);
	}
}

void AHeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;

	// 1. 적 탐색
	ScanForTarget();

	// 2. 평타 (사거리 내 적 있으면 무조건 발사)
	PerformAutoBasicAttack();

	// 3. 스킬 (자동 모드일 때만)
	if (bIsAutoSkillMode)
	{
		PerformAutoSkill();
	}
}


void AHeroCharacter::ScanForTarget()
{
	// 기존 타겟이 유효하면 거리 체크만 하고 리턴 (최적화)
	if (CurrentTarget && !CurrentTarget->IsDead())
	{
		float DistSq = FVector::DistSquared(GetActorLocation(), CurrentTarget->GetActorLocation());
		if (DistSq <= (AttackRange * AttackRange)) return;
	}

	// 구체 범위 내 적 탐색
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	TArray<AActor*> OutActors;

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), AttackRange, ObjectTypes, ABaseCharacter::StaticClass(), IgnoreActors, OutActors);

	float MinDist = FLT_MAX;
	CurrentTarget = nullptr;

	for (AActor* Actor : OutActors)
	{
		ABaseCharacter* Candidate = Cast<ABaseCharacter>(Actor);
		// 적군 확인 (TeamID 다름) + 생존 확인
		if (Candidate && Candidate->TeamID != this->TeamID && !Candidate->IsDead())
		{
			float Dist = FVector::DistSquared(GetActorLocation(), Candidate->GetActorLocation());
			if (Dist < MinDist)
			{
				MinDist = Dist;
				CurrentTarget = Candidate;
			}
		}
	}
}

void AHeroCharacter::PerformAutoBasicAttack()
{
	if (!CurrentTarget || !AbilitySystemComponent) return;

	// 적 방향 바라보기 (무빙샷 연출)
	FVector LookDir = CurrentTarget->GetActorLocation() - GetActorLocation();
	LookDir.Z = 0.0f;
	if (!LookDir.IsNearlyZero())
	{
		SetActorRotation(LookDir.Rotation());
	}

	// GAS 평타 실행 시도 (쿨타임은 GAS가 관리)
	FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Ability.Action.Attack"));
	AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
}

void AHeroCharacter::PerformAutoSkill()
{
	if (!AbilitySystemComponent) return;

	// 예: 1번 스킬 자동 사용
	FGameplayTag SkillTag = FGameplayTag::RequestGameplayTag(FName("Ability.Action.Skill1"));
	AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(SkillTag));
}

void AHeroCharacter::ToggleAutoSkillMode()
{
	bIsAutoSkillMode = !bIsAutoSkillMode;
}

void AHeroCharacter::Die()
{
	//1. 이미 사망 상태면 무시
	if (IsDead()) return;

	//2. 부모 클래스에서 사망 처리
	Super::Die();

	//3. 게임모드에 리스폰 요청
	if (ABaseGameMode* GM = Cast<ABaseGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->OnHeroDied(GetController());
	}
}