#include "Controller/HeroController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/HeroCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Mode/BaseGameplayTags.h"

AHeroController::AHeroController()
{
	// 마우스 커서는 보이게 (UI 클릭 등을 위해)
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void AHeroController::BeginPlay()
{
	Super::BeginPlay();

	// IMC 등록
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AHeroController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// 스킬 입력 바인딩 (Started: 누르는 순간)
		if (Skill1Action)
		{
			EnhancedInputComponent->BindAction(Skill1Action, ETriggerEvent::Started, this, &AHeroController::OnSkill1Triggered);
		}
	}
}

void AHeroController::OnSkill1Triggered()
{
	// 자동 스킬 모드면 수동 입력 무시 (선택사항)
	AHeroCharacter* Hero = Cast<AHeroCharacter>(GetPawn());
	if (Hero && Hero->bIsAutoSkillMode) return;

	// 수동으로 스킬 1 발동
	if (Hero)
	{
		IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Hero);
		if (ASCInterface)
		{
			UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
			if (ASC)
			{
				FGameplayTag SkillTag = FBaseGameplayTags::Get().Ability_Action_Skill1;
				ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SkillTag));
			}
		}
	}
}