#include "LinkageCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ALinkageCharacter::ALinkageCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(34.f, 88.f);
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    Camera->SetupAttachment(GetCapsuleComponent());
    Camera->SetRelativeLocation(FVector(0, 0, 64));
    Camera->bUsePawnControlRotation = true;
    Camera->FieldOfView = 90.f;
    GetCharacterMovement()->MaxWalkSpeed = 450.f;
    GetCharacterMovement()->JumpZVelocity = 420.f;
    GetCharacterMovement()->AirControl = 0.25f;
    bUseControllerRotationYaw = true;

    Mapping = CreateDefaultSubobject<UInputMappingContext>(TEXT("DevelopmentControls"));
    MoveForward = CreateDefaultSubobject<UInputAction>(TEXT("MoveForward"));
    MoveRight = CreateDefaultSubobject<UInputAction>(TEXT("MoveRight"));
    LookYaw = CreateDefaultSubobject<UInputAction>(TEXT("LookYaw"));
    LookPitch = CreateDefaultSubobject<UInputAction>(TEXT("LookPitch"));
    for (UInputAction* Action : {MoveForward.Get(), MoveRight.Get(), LookYaw.Get(), LookPitch.Get()})
        Action->ValueType = EInputActionValueType::Axis1D;
    JumpAction = CreateDefaultSubobject<UInputAction>(TEXT("Jump"));
    ResetAction = CreateDefaultSubobject<UInputAction>(TEXT("ResetRoom"));
    Mapping->MapKey(MoveForward, EKeys::W);
    Mapping->MapKey(MoveRight, EKeys::D);
    Mapping->MapKey(MoveForward, EKeys::S).Modifiers.Add(CreateDefaultSubobject<UInputModifierNegate>(TEXT("Backward")));
    Mapping->MapKey(MoveRight, EKeys::A).Modifiers.Add(CreateDefaultSubobject<UInputModifierNegate>(TEXT("Left")));
    Mapping->MapKey(LookYaw, EKeys::MouseX);
    Mapping->MapKey(LookPitch, EKeys::MouseY).Modifiers.Add(CreateDefaultSubobject<UInputModifierNegate>(TEXT("InvertMouseY")));
    Mapping->MapKey(JumpAction, EKeys::SpaceBar);
    Mapping->MapKey(ResetAction, EKeys::R);
}

void ALinkageCharacter::PawnClientRestart()
{
    Super::PawnClientRestart();
    if (const APlayerController* PC = Cast<APlayerController>(Controller))
        if (ULocalPlayer* Player = PC->GetLocalPlayer())
            if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Player))
            {
                Subsystem->RemoveMappingContext(Mapping);
                Subsystem->AddMappingContext(Mapping, 0);
            }
}

void ALinkageCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
    Super::SetupPlayerInputComponent(Input);
    if (auto* Enhanced = Cast<UEnhancedInputComponent>(Input))
    {
        Enhanced->BindAction(MoveForward, ETriggerEvent::Triggered, this, &ALinkageCharacter::Forward);
        Enhanced->BindAction(MoveRight, ETriggerEvent::Triggered, this, &ALinkageCharacter::Right);
        Enhanced->BindAction(LookYaw, ETriggerEvent::Triggered, this, &ALinkageCharacter::Yaw);
        Enhanced->BindAction(LookPitch, ETriggerEvent::Triggered, this, &ALinkageCharacter::Pitch);
        Enhanced->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        Enhanced->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
        Enhanced->BindAction(ResetAction, ETriggerEvent::Started, this, &ALinkageCharacter::ResetRoom);
    }
}

void ALinkageCharacter::Forward(const FInputActionValue& Value) { AddMovementInput(GetActorForwardVector(), Value.Get<float>()); }
void ALinkageCharacter::Right(const FInputActionValue& Value) { AddMovementInput(GetActorRightVector(), Value.Get<float>()); }
void ALinkageCharacter::Yaw(const FInputActionValue& Value) { AddControllerYawInput(Value.Get<float>()); }
void ALinkageCharacter::Pitch(const FInputActionValue& Value) { AddControllerPitchInput(Value.Get<float>()); }

void ALinkageCharacter::ResetRoom()
{
    UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this, true)));
}
