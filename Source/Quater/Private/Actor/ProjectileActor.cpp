


#include "Actor/ProjectileActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Character/BaseCharacter.h" // TeamID 확인용


AProjectileActor::AProjectileActor()
{
 	
	PrimaryActorTick.bCanEverTick = false;

    // 1. 충돌체
    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    SetRootComponent(SphereComp);
    SphereComp->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // 겹침 감지

    // 2. 메쉬
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootComponent);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 3. 무브먼트 (직선으로 날아감)
    MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
    MovementComp->InitialSpeed = 1000.0f;
    MovementComp->MaxSpeed = 1000.0f;
    MovementComp->ProjectileGravityScale = 0.0f; // 중력 영향 없음 (직사)

    //4. 유도 미사일 설정
    MovementComp->bIsHomingProjectile = true; // 유도 활성화
    MovementComp->HomingAccelerationMagnitude = 10000.0f; // 유도 성능 (클수록 급격하게 꺾임)

    // 수명 (3초 뒤 자동 삭제)
    InitialLifeSpan = 3.0f;
}

// 발사 후 타겟 지정 필요
void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	
    // 타겟을 향해 날아가도록 설정 (주인이 타겟 정보를 넘겨줘야 함)
    // 이 부분은 생성 후 SetHomingTargetComponent를 호출해야 하므로 캐릭터 쪽에서 구현해야 함.
    
}

void AProjectileActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner()) return;

    // 1. 팀 확인 (같은 팀이면 통과)
    ABaseCharacter* TargetChar = Cast<ABaseCharacter>(OtherActor);
    if (TargetChar && (uint8)TargetChar->TeamID == MyTeamID) return;

    // 2. GAS 데미지 적용
    IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor);
    if (ASCInterface && DamageEffectClass)
    {
        UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
        if (TargetASC)
        {
            FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
            Context.AddInstigator(GetOwner(), this);

            TargetASC->ApplyGameplayEffectToSelf(DamageEffectClass.GetDefaultObject(), 1.0f, Context);
        }
    }

    // 3. 명중 후 삭제
    Destroy();
}

