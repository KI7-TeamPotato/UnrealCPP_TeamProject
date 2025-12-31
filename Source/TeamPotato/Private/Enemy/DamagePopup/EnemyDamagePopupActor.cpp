

#include "Enemy/DamagePopup/EnemyDamagePopupActor.h"
#include "Components/WidgetComponent.h"

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
	
}


