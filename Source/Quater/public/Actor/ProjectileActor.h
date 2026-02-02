// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h" // GAS 효과 적용
#include "ProjectileActor.generated.h"

UCLASS()
class QUATER_API AProjectileActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AProjectileActor();

protected:
	
	virtual void BeginPlay() override;

public:	
    // 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class USphereComponent* SphereComp; // 충돌체

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UProjectileMovementComponent* MovementComp; // 날아가는 움직임

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UStaticMeshComponent* MeshComp; // 화살/마법구 모양

    // 설정값
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float Range = 1000.0f;

    // 데미지 효과 (GAS)
    UPROPERTY(EditDefaultsOnly, Category = "GAS")
    TSubclassOf<UGameplayEffect> DamageEffectClass;

    // 아군/적군 구분 (팀킬 방지) 발사체 통과 기준 확인용
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    uint8 MyTeamID;

    // 발사한 주인 (Instigator)
    UPROPERTY()
    AActor* OwnerActor;

    // 충돌 시 처리
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

};
