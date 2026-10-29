#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LinkageGameMode.generated.h"
UCLASS()
class LINKAGE_API ALinkageGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ALinkageGameMode();
    virtual void BeginPlay() override;
};
