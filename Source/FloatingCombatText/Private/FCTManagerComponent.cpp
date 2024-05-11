// Fill out your copyright notice in the Description page of Project Settings.


#include "FCTManagerComponent.h"
#include "Engine/Canvas.h"
#include "Curves/CurveVector.h"
#include "Curves/CurveLinearColor.h"
#include "Engine/Font.h"
#include "Runtime/Engine/Classes/GameFramework/HUD.h"

// Sets default values for this component's properties
UFCTManagerComponent::UFCTManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

bool UFCTManagerComponent::IsFloatingCombatTextExpired(FFCTData const& CombatText) const
{
	return CombatText.Duration >= AnimationTemplates[CombatText.GetTemplateIndex()].BaseDuration;
}

void UFCTManagerComponent::DrawFloatingCombatText(FFCTData const& CombatText, UCanvas* Canvas)
{
	const FFloatingCombatTextAnimationTemplate& AnimTemplate = AnimationTemplates[CombatText.GetTemplateIndex()];
	const UCurveVector* PositionCurve = AnimTemplate.PositionAnimationCurve;

	if (!PositionCurve)
	{
		return;
	}

	FVector const CurrentTextPosition = CombatText.StartPoint + PositionCurve->GetVectorValue(CombatText.Duration);

	/** Transforms our 3D world space vector to 2D screen coordinates. */
	FVector Location2D = Canvas->Project(CurrentTextPosition);

	/** TODO: Fix check X and Y coordinates. */
	if (!Location2D.Z)
	{
		/** We don't need to draw, if number is behind the screen. */
		return;
	}

	const UCurveLinearColor* ColorCurve = AnimTemplate.ColorAnimationCurve;
	const UCurveFloat* TextSizeCurve = AnimTemplate.TextSizeAnimationCurve;
	const UFont* CombatTextFont = AnimTemplate.CombatTextFont;

	if (!ColorCurve || !TextSizeCurve || !CombatTextFont)
	{
		return;
	}

	const float         Scale = TextSizeCurve->GetFloatValue(CombatText.Duration);
	const FLinearColor  Color = ColorCurve->GetLinearColorValue(CombatText.Duration);

	float OutWidth = 0.f, OutHeight = 0.f;

	/** Extracting text size on screen. This is for text pivot. */
	Canvas->TextSize(CombatTextFont, CombatText.FloatingTextInString, OutWidth, OutHeight, Scale, Scale);

	/** The pivot is on down center. */
	Location2D.X -= OutWidth / 2.f;
	Location2D.Y -= OutHeight;

	FVector2D ShadowPosition;
	const FLinearColor ShadowColor(0, 0, 0, 0.5f * Color.A);

	ShadowPosition.X = Location2D.X + 2.f;
	ShadowPosition.Y = Location2D.Y - 2.f;

	FCanvasTextItem TextItem(ShadowPosition, CombatText.FloatingText, CombatTextFont, ShadowColor);

	TextItem.Scale.X = Scale;
	TextItem.Scale.Y = Scale;

	/** Drawing shadow. */
	Canvas->DrawItem(TextItem);

	TextItem.Position = FVector2D(Location2D);
	TextItem.SetColor(Color);

	/** Drawing text. */
	Canvas->DrawItem(TextItem);
}

// Called when the game starts
void UFCTManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	AHUD::OnHUDPostRender.AddUFunction(this, "DrawAll");
}

// Called every frame
void UFCTManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Update(DeltaTime);

	if (bSimulationEnabled)
	{
		for (int Index = 0; Index < NewElementsInTick; Index++)
		{
			const int32 Number = FMath::RandRange(SimNumberRange.X, SimNumberRange.Y);
			FBox Box;
			Box.Min = MinPosition;
			Box.Max = MaxPosition;

			CreateFloatingCombatText(Number, FMath::RandPointInBox(Box), FMath::RandRange(0, AnimationTemplates.Num() - 1));
		}

		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT("AllFloatingCombatTexts size: %u"), AllFloatingCombatTexts.Num()));
	}
}

void UFCTManagerComponent::Update(float Delta)
{
	for (int32 ItemIndex = 0; ItemIndex < AllFloatingCombatTexts.Num();)
	{
		FFCTData& CurrentFCT = AllFloatingCombatTexts.GetData()[ItemIndex];

		if (IsFloatingCombatTextExpired(CurrentFCT))
		{
			AllFloatingCombatTexts.RemoveAtSwap(ItemIndex);
			continue;
		}

		CurrentFCT.Duration += Delta;
		ItemIndex++;
	}
}

void UFCTManagerComponent::DrawAll(AHUD* HUD, UCanvas* Canvas)
{
	for (FFCTData const& CurrentFCT : AllFloatingCombatTexts)
	{
		DrawFloatingCombatText(CurrentFCT, Canvas);
	}
}

void UFCTManagerComponent::CreateFloatingCombatText(FText const& NewText, FVector const& DrawPoint, uint8 TemplateIndex)
{
	FFCTData NewFloatingText;

	NewFloatingText.FloatingText = NewText;
	NewFloatingText.FloatingTextInString = NewText.ToString();
	NewFloatingText.TemplateIndex = TemplateIndex;
	NewFloatingText.StartPoint = DrawPoint;

	AllFloatingCombatTexts.Add(NewFloatingText);
}

void UFCTManagerComponent::CreateFloatingCombatText(int32 Value, FVector const& DrawPoint, uint8 TemplateIndex)
{
	CreateFloatingCombatText(FText::AsNumber(Value), DrawPoint, TemplateIndex);
}

