#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CameraManager.generated.h"

UCLASS()
class UE5GROUPPROJECT_API ACameraManager : public AActor
{
    GENERATED_BODY()

public:
    ACameraManager();

    // Switch between top-down and 3D perspective
    void ToggleCamera();

    // Top-down camera controls
    void MoveCamera(FVector Direction);
    void ZoomCamera(float Value);

    // 3D camera controls
    void RotateCamera(float Yaw, float Pitch);
    void PanCamera3D(FVector Direction);
    void Zoom3DCamera(float Value);

    UPROPERTY(VisibleAnywhere)
    USceneComponent* RootScene;

    // Top-down orthographic camera
    UPROPERTY(VisibleAnywhere, Category = "Camera|TopDown")
    UCameraComponent* TopDownCamera;

    // 3D perspective spring arm
    UPROPERTY(VisibleAnywhere, Category = "Camera|3D")
    USpringArmComponent* SpringArm;

    // 3D perspective camera
    UPROPERTY(VisibleAnywhere, Category = "Camera|3D")
    UCameraComponent* PerspectiveCamera;

    // Top-down movement speed
    UPROPERTY(EditAnywhere, Category = "Camera|TopDown")
    float PanSpeed = 1500.f;

    // Top-down zoom speed
    UPROPERTY(EditAnywhere, Category = "Camera|TopDown")
    float ZoomSpeed = 6000.f;

    // Top-down zoom limits
    UPROPERTY(EditAnywhere, Category = "Camera|TopDown")
    float MinOrthoWidth = 512.f;

    UPROPERTY(EditAnywhere, Category = "Camera|TopDown")
    float MaxOrthoWidth = 4096.f;

    // 3D pan speed
    UPROPERTY(EditAnywhere, Category = "Camera|3D")
    float PanSpeed3D = 2000.f;

    // 3D rotation speed
    UPROPERTY(EditAnywhere, Category = "Camera|3D")
    float RotateSpeed = 300.f;

    // 3D zoom limits
    UPROPERTY(EditAnywhere, Category = "Camera|3D")
    float MinArmLength = 500.f;

    UPROPERTY(EditAnywhere, Category = "Camera|3D")
    float MaxArmLength = 5000.f;

    // 3D zoom speed
    UPROPERTY(EditAnywhere, Category = "Camera|3D")
    float ZoomSpeed3D = 700.f;

    bool bIsTopDown = true;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
};