// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyCharacter.h"

AMyAIController::AMyAIController()
{
    // Perception 컴포넌트 생성
    AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
    SetPerceptionComponent(*AIPerceptionComp);

    //시각 설정 블루프린트랑 같음
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 1000.0f;
    SightConfig->LoseSightRadius = 1200.0f;
    SightConfig->PeripheralVisionAngleDegrees = 60.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    AIPerceptionComp->ConfigureSense(*SightConfig);

    //피격 설정 이것도 블루프린트에 있음
    DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
    AIPerceptionComp->ConfigureSense(*DamageConfig);

    // 지배적? 감각 설정
    AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AMyAIController::BeginPlay()
{
    Super::BeginPlay();
    // 델리게이트 바인딩 (블루프린트의 OnTargetPerceptionUpdated 이벤트와 동일)
    if (AIPerceptionComp)
    {
        AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AMyAIController::OnTargetPerceptionUpdated);
    }
}

void AMyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // BT 실행 (캐릭터나 컨트롤러에 BT가 할당되어 있다고 가정)
    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(InPawn);
    //*실제로는 Enemy->BehaviorTreeAsset 등을 가져와서 RunBehaviorTree 해야 함.
}

EEnemyState AMyAIController::GetCurrentState() const
{
    //블랙보드에 있는 현재 상태를 가져옴
    if (Blackboard)
    {
        return (EEnemyState)Blackboard->GetValueAsEnum(Key_State);
    }
    return EEnemyState::Patrol;
}

void AMyAIController::SetStateAsAttack(AActor* TargetActor)
{
    //블랙보드에서 어택 상태로 바꿈
    if (Blackboard)
    {
        Blackboard->SetValueAsObject(Key_Target, TargetActor);
        Blackboard->SetValueAsEnum(Key_State, (uint8)EEnemyState::Attack);
    }
}

void AMyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (Actor != PlayerChar) return;

    //감각없으면 리턴
    if (!Stimulus.WasSuccessfullySensed()) return;

    //현재 상태 확인 하고 idle이나 patrol이면 attack으로 전환 다른 상태에선 무시
    EEnemyState CurrentState = GetCurrentState();

    if (CurrentState == EEnemyState::Idle || CurrentState == EEnemyState::Patrol)
    {
        SetStateAsAttack(Actor);
    }
    // 이미 Attack, Die, Hit 상태라면 무시
}
