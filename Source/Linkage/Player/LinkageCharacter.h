#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "LinkageCharacter.generated.h"

class UCameraComponent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class LINKAGE_API ALinkageCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    ALinkageCharacter();
    virtual void PawnClientRestart() override;
    UCameraComponent* GetFirstPersonCamera() const { return Camera; }
protected:
    virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
private:
    void Forward(const FInputActionValue& Value);
    void Right(const FInputActionValue& Value);
    void Yaw(const FInputActionValue& Value);
    void Pitch(const FInputActionValue& Value);
    void ResetRoom();
    UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraComponent> Camera;
    UPROPERTY() TObjectPtr<UInputMappingContext> Mapping;
    UPROPERTY() TObjectPtr<UInputAction> MoveForward;
    UPROPERTY() TObjectPtr<UInputAction> MoveRight;
    UPROPERTY() TObjectPtr<UInputAction> LookYaw;
    UPROPERTY() TObjectPtr<UInputAction> LookPitch;
    UPROPERTY() TObjectPtr<UInputAction> JumpAction;
    UPROPERTY() TObjectPtr<UInputAction> ResetAction;
};
