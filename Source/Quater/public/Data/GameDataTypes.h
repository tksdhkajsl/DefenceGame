#pragma once

#include "CoreMinimal.h"



// --- [1] 물리 충돌 채널 매크로 정의 (Project Settings -> Collision과 일치시켜야 함) ---
// 예: Trace Channel 1은 '무기 공격 판정'용, Object Channel 1은 '투사체'용
#define CCHANNEL_GAME_WEAPON  ECC_GameTraceChannel1
#define CCHANNEL_GAME_PROJECTILE ECC_GameTraceChannel2

// --- [2] 팀 구분 (피아 식별용) ---
// 블루프린트 에디터에서 드롭다운으로 선택 가능
UENUM(BlueprintType)
enum class ETeamType : uint8
{
    None        UMETA(DisplayName = "None"),
    Player      UMETA(DisplayName = "Player Hero"), // 플레이어 영웅
    Ally        UMETA(DisplayName = "Ally Unit"),   // 아군 소환수
    Enemy       UMETA(DisplayName = "Enemy Unit"),  // 적군
    Neutral     UMETA(DisplayName = "Neutral")      // 중립 (오브젝트 등)
};

// --- [3] 유닛 역할군 (AI 타겟팅 우선순위용) ---
UENUM(BlueprintType)
enum class EUnitRole : uint8
{
    Melee       UMETA(DisplayName = "Melee (Tank/Warrior)"), // 근거리 (우선 공격 대상)
    Ranged      UMETA(DisplayName = "Ranged (Archer/Mage)"), // 원거리
    Tanker     UMETA(DisplayName = "Tanker (Tanker)"),     // 탱커 (1순위 제거 대상)
    Boss        UMETA(DisplayName = "Boss")                  // 보스
};

// --- [4] 장비 슬롯 (플레이어 장비 교체용) ---
UENUM(BlueprintType)
enum class EItemSlot : uint8
{
    Weapon      UMETA(DisplayName = "Weapon"),  // 무기 (스킬 변경 핵심)
    Helmet      UMETA(DisplayName = "Helmet"),  // 투구
    Armor       UMETA(DisplayName = "Armor"),   // 갑옷
    Accessory   UMETA(DisplayName = "Accessory") // 장신구
};

// --- [5] 공격 판정 결과 (GAS 데미지 계산용) ---
UENUM(BlueprintType)
enum class EHitResultType : uint8
{
    Normal      UMETA(DisplayName = "Normal Hit"),
    Critical    UMETA(DisplayName = "Critical Hit"), // 치명타
    Block       UMETA(DisplayName = "Blocked"),      // 막힘
    Miss        UMETA(DisplayName = "Miss")          // 빗나감 (회피)
};