// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FCTManagerComponent.generated.h"

class UCurveVector;
class UCurveLinearColor;
class UCurveFloat;

USTRUCT()
struct FFCTData
{
    GENERATED_BODY()

private:

    friend class UFCTManagerComponent;

    uint8 TemplateIndex;

    float Duration;

    FVector StartPoint;

    FText FloatingText;

    FString FloatingTextInString;

public:

    FORCEINLINE uint8 GetTemplateIndex() const
    {
        return TemplateIndex;
    }
};

USTRUCT()
struct FFloatingCombatTextAnimationTemplate
{
    GENERATED_BODY()

private:

    friend class UFCTManagerComponent;

    UPROPERTY(EditAnywhere)
    UCurveVector* PositionAnimationCurve;

    UPROPERTY(EditAnywhere)
    UCurveLinearColor* ColorAnimationCurve;

    UPROPERTY(EditAnywhere)
    UCurveFloat* TextSizeAnimationCurve;

    UPROPERTY(EditAnywhere)
    UFont* CombatTextFont;

    UPROPERTY(EditAnywhere)
    float BaseDuration;
};

/** Floating Combat Text Manager Component */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName = "Floating Combat Text Manager Component"),
	   hideCategories = (Tags, ComponentTick, ComponentReplication, Replication, Navigation) )
class FLOATINGCOMBATTEXT_API UFCTManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFCTManagerComponent();

private:

    UPROPERTY(EditAnywhere)
    TArray<FFloatingCombatTextAnimationTemplate> AnimationTemplates;

    UPROPERTY()
    TArray<FFCTData> AllFloatingCombatTexts;

	bool bSimulationEnabled = false;

	UPROPERTY(EditAnywhere, Category = "Debug")
	FVector2D SimNumberRange;

	UPROPERTY(EditAnywhere, Category = "Debug")
	FVector MinPosition;

	UPROPERTY(EditAnywhere, Category = "Debug")
	FVector MaxPosition;

	/** How many elements will be created in one tick? */
	UPROPERTY(EditAnywhere, Category = "Debug")
	uint8 NewElementsInTick = 10;

	TWeakObjectPtr<UCanvas> CanvasPtr;

    bool IsFloatingCombatTextExpired(FFCTData const& CombatText) const;

    void DrawFloatingCombatText(FFCTData const& CombatText, UCanvas* Canvas);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable)
	void SetSimulationEnabled(bool bIsEnable) { bSimulationEnabled = bIsEnable; }

	
	UFUNCTION()
	void DrawAll(AHUD* HUD, UCanvas* Canvas);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Update(float Delta);


	void CreateFloatingCombatText(FText const& NewText, FVector const& DrawPoint, uint8 TemplateIndex);
	void CreateFloatingCombatText(int32 Value, FVector const& DrawPoint, uint8 TemplateIndex);
	
};
