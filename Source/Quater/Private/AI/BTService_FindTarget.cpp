// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_FindTarget.h"
#include "AIController.h"
#include "Character/BaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/BaseCharacter.h"
#include "Pawn/BaseStructure.h"

UBTService_FindTarget::UBTService_FindTarget()
{
	NodeName = TEXT("Find Nearest Target");
	Interval = 0.5f; // 0.5초마다 탐색
}

void UBTService_FindTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();
	ABaseCharacter* MyChar = Cast<ABaseCharacter>(OwningPawn);
	if (!MyChar) return;

	// Sphere Overlap으로 주변 검색
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwningPawn);

	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		MyChar->GetActorLocation(),
		DetectRange,
		ObjectTypes,
		ABaseCharacter::StaticClass(),
		ActorsToIgnore,
		OutActors
	);

	ABaseCharacter* NearestTarget = nullptr;
	float MinDistanceSq = FLT_MAX;

	for (AActor* Actor : OutActors)
	{
		ABaseCharacter* Target = Cast<ABaseCharacter>(Actor);
		// 1. 적군인지 확인 (TeamID 다름)
		// 2. 살아있는지 확인 (!IsDead)
		if (Target && Target->TeamID != MyChar->TeamID && !Target->IsDead())
		{
			float DistSq = FVector::DistSquared(Target->GetActorLocation(), MyChar->GetActorLocation());
			if (DistSq < MinDistanceSq)
			{
				MinDistanceSq = DistSq;
				NearestTarget = Target;
			}
		}
	}

	// 블랙보드에 결과 저장 (없으면 null 저장됨)
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, NearestTarget);
}
