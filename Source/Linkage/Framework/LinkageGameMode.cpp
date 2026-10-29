#include "LinkageGameMode.h"
#include "../Player/LinkageCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "HAL/PlatformMisc.h"
#include "TimerManager.h"

ALinkageGameMode::ALinkageGameMode() { DefaultPawnClass = ALinkageCharacter::StaticClass(); }

void ALinkageGameMode::BeginPlay()
{
    Super::BeginPlay();
    if (FParse::Param(FCommandLine::Get(), TEXT("LinkageSmoke")))
    {
        FTimerHandle Handle;
        GetWorldTimerManager().SetTimer(Handle, [this]()
        {
            const auto* Pawn = Cast<ALinkageCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
            const bool bOK = Pawn && Pawn->IsPlayerControlled() && Pawn->GetFirstPersonCamera()->IsActive()
                && Pawn->GetCharacterMovement()->IsMovingOnGround() && GetWorld()->GetMapName().Contains(TEXT("L_Development"));
            UE_LOG(LogTemp, Display, TEXT("LINKAGE_SMOKE %s: first-person pawn, possession, floor and development map"), bOK ? TEXT("PASS") : TEXT("FAIL"));
            FPlatformMisc::RequestExitWithStatus(false, bOK ? 0 : 1);
        }, 3.f, false);
    }
}
