

#include "Enemy/DamagePopup/EnemyDamagePopupActor.h"
#include "Components/WidgetComponent.h"
#include "UI/Enemy/EnemyDamagePopupWidget.h"
#include "Subsystem/PoolingSubsystem.h"

AEnemyDamagePopupActor::AEnemyDamagePopupActor()
{
	PrimaryActorTick.bCanEverTick = false;

    DamagePopupWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamagePopupWidgetComponent"));
    DamagePopupWidgetComponent->SetupAttachment(RootComponent);
    DamagePopupWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
}

void AEnemyDamagePopupActor::BeginPlay()
{
    Super::BeginPlay();

    if (DamagePopupWidgetComponent->GetWidget())
    {
        DamagePopupWidget = Cast<UEnemyDamagePopupWidget>(DamagePopupWidgetComponent->GetWidget());

        DamagePopupWidget->RequestReturnPoolDelegate.BindDynamic(this, &AEnemyDamagePopupActor::HideDamagePopup);
    }
}

void AEnemyDamagePopupActor::ShowDamagePopup(float DamageAmount, FVector Location)
{
    // 액터 위치 설정
    SetActorLocation(Location);

    if (DamagePopupWidget)
    {
        DamagePopupWidget->SetActivateDamagePopup(DamageAmount);
    }
}

void AEnemyDamagePopupActor::HideDamagePopup()
{
    UPoolingSubsystem* PoolingSubsystem = GetWorld()->GetSubsystem<UPoolingSubsystem>();
    if (PoolingSubsystem)
    {
        PoolingSubsystem->ReturnDamagePopupActorToPool(this);
    }
}
