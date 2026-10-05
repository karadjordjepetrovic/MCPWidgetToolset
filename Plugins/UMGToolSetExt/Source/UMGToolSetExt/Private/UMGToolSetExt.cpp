// Copyright Epic Games, Inc. All Rights Reserved.

#include "UMGToolSetExt.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/Texture2D.h"
#include "UObject/Package.h"
#include "UObject/SoftObjectPath.h"
#include "WidgetBlueprint.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "EdGraphSchema_K2.h"
#include "GameFramework/Actor.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Event.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

namespace
{
	UTexture2D* GetWhiteSquareTexture()
	{
		static const FSoftObjectPath WhiteSquarePath(TEXT("/Engine/EngineResources/WhiteSquareTexture.WhiteSquareTexture"));
		return Cast<UTexture2D>(WhiteSquarePath.TryLoad());
	}
}

bool UUMGToolSetExt::SetSolidColor(UWidgetBlueprint* WidgetBlueprint, UWidget* Widget, FLinearColor Color)
{
	if (!WidgetBlueprint || !Widget)
	{
		return false;
	}

	UTexture2D* WhiteTexture = GetWhiteSquareTexture();
	if (!WhiteTexture)
	{
		return false;
	}

	if (UImage* Image = Cast<UImage>(Widget))
	{
		FSlateBrush Brush = Image->GetBrush();
		Brush.SetResourceObject(WhiteTexture);
		Brush.DrawAs = ESlateBrushDrawType::Image;
		Brush.ImageType = ESlateBrushImageType::FullColor;
		Image->SetBrush(Brush);
		Image->SetColorAndOpacity(Color);

		WidgetBlueprint->MarkPackageDirty();
		return true;
	}

	if (UBorder* Border = Cast<UBorder>(Widget))
	{
		FSlateBrush Brush = Border->Background;
		Brush.SetResourceObject(WhiteTexture);
		Brush.DrawAs = ESlateBrushDrawType::Image;
		Brush.ImageType = ESlateBrushImageType::FullColor;
		Border->SetBrush(Brush);
		Border->SetBrushColor(Color);

		WidgetBlueprint->MarkPackageDirty();
		return true;
	}

	return false;
}

bool UUMGToolSetExt::SetAnchorPreset(UWidgetBlueprint* WidgetBlueprint, UWidget* Widget, EUMGAnchorPreset Preset, FVector2D Size, FMargin Margin)
{
	if (!WidgetBlueprint || !Widget)
	{
		return false;
	}

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot);
	if (!CanvasSlot)
	{
		return false;
	}

	if (Preset == EUMGAnchorPreset::StretchFill)
	{
		CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		CanvasSlot->SetAlignment(FVector2D(0.f, 0.f));
		CanvasSlot->SetOffsets(Margin);

		WidgetBlueprint->MarkPackageDirty();
		return true;
	}

	float AnchorX = 0.5f;
	float AnchorY = 0.5f;
	switch (Preset)
	{
	case EUMGAnchorPreset::TopLeft:      AnchorX = 0.f;  AnchorY = 0.f;  break;
	case EUMGAnchorPreset::TopCenter:    AnchorX = 0.5f; AnchorY = 0.f;  break;
	case EUMGAnchorPreset::TopRight:     AnchorX = 1.f;  AnchorY = 0.f;  break;
	case EUMGAnchorPreset::CenterLeft:   AnchorX = 0.f;  AnchorY = 0.5f; break;
	case EUMGAnchorPreset::Center:       AnchorX = 0.5f; AnchorY = 0.5f; break;
	case EUMGAnchorPreset::CenterRight:  AnchorX = 1.f;  AnchorY = 0.5f; break;
	case EUMGAnchorPreset::BottomLeft:   AnchorX = 0.f;  AnchorY = 1.f;  break;
	case EUMGAnchorPreset::BottomCenter: AnchorX = 0.5f; AnchorY = 1.f;  break;
	case EUMGAnchorPreset::BottomRight:  AnchorX = 1.f;  AnchorY = 1.f;  break;
	default: break;
	}

	// With a point anchor, Offsets.Left/Top is the position of the widget's Alignment pivot
	// relative to the anchor point. Alignment == anchor point means: for an edge/corner this
	// side touches, insetting by Margin should move *inward*, i.e. negative on the max side.
	const float PosX = (AnchorX >= 1.f) ? -Margin.Right : (AnchorX <= 0.f ? Margin.Left : 0.f);
	const float PosY = (AnchorY >= 1.f) ? -Margin.Bottom : (AnchorY <= 0.f ? Margin.Top : 0.f);

	CanvasSlot->SetAnchors(FAnchors(AnchorX, AnchorY, AnchorX, AnchorY));
	CanvasSlot->SetAlignment(FVector2D(AnchorX, AnchorY));
	CanvasSlot->SetOffsets(FMargin(PosX, PosY, Size.X, Size.Y));

	WidgetBlueprint->MarkPackageDirty();
	return true;
}

bool UUMGToolSetExt::ResizeCanvasSlot(UWidgetBlueprint* WidgetBlueprint, UWidget* Widget, FVector2D NewSize)
{
	if (!WidgetBlueprint || !Widget)
	{
		return false;
	}

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot);
	if (!CanvasSlot)
	{
		return false;
	}

	CanvasSlot->SetSize(NewSize);

	WidgetBlueprint->MarkPackageDirty();
	return true;
}

UBlueprint* UUMGToolSetExt::CreateWidgetDisplayActor(const FString& WidgetBlueprintPath, const FString& FolderPath, const FString& ActorAssetName)
{
	UWidgetBlueprint* WidgetBP = LoadObject<UWidgetBlueprint>(nullptr, *WidgetBlueprintPath);
	if (!WidgetBP || !WidgetBP->GeneratedClass)
	{
		return nullptr;
	}
	TSubclassOf<UUserWidget> WidgetClass = *WidgetBP->GeneratedClass;

	const FString PackagePath = FolderPath / ActorAssetName;
	UPackage* Package = CreatePackage(*PackagePath);
	UBlueprint* NewBP = FKismetEditorUtilities::CreateBlueprint(
		AActor::StaticClass(), Package, FName(*ActorAssetName), BPTYPE_Normal,
		UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
	if (!NewBP)
	{
		return nullptr;
	}
	FAssetRegistryModule::AssetCreated(NewBP);

	UEdGraph* EventGraph = FBlueprintEditorUtils::FindEventGraph(NewBP);
	if (!EventGraph)
	{
		return nullptr;
	}

	// Event BeginPlay
	UK2Node_Event* BeginPlayNode = NewObject<UK2Node_Event>(EventGraph);
	BeginPlayNode->EventReference.SetExternalMember(FName(TEXT("ReceiveBeginPlay")), AActor::StaticClass());
	BeginPlayNode->bOverrideFunction = true;
	BeginPlayNode->NodePosX = 0;
	BeginPlayNode->NodePosY = 0;
	EventGraph->AddNode(BeginPlayNode, false, false);
	BeginPlayNode->CreateNewGuid();
	BeginPlayNode->PostPlacedNewNode();
	BeginPlayNode->AllocateDefaultPins();

	// Create Widget
	UFunction* CreateWidgetFn = UWidgetBlueprintLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UWidgetBlueprintLibrary, Create));
	UK2Node_CallFunction* CreateWidgetNode = NewObject<UK2Node_CallFunction>(EventGraph);
	CreateWidgetNode->NodePosX = 300;
	CreateWidgetNode->NodePosY = 0;
	EventGraph->AddNode(CreateWidgetNode, false, false);
	CreateWidgetNode->SetFromFunction(CreateWidgetFn);
	CreateWidgetNode->CreateNewGuid();
	CreateWidgetNode->PostPlacedNewNode();
	CreateWidgetNode->AllocateDefaultPins();

	UEdGraphPin* ClassPin = CreateWidgetNode->FindPinChecked(TEXT("WidgetType"));
	ClassPin->DefaultObject = WidgetClass.Get();

	// Add to Viewport
	UFunction* AddToViewportFn = UUserWidget::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UUserWidget, AddToViewport));
	UK2Node_CallFunction* AddToViewportNode = NewObject<UK2Node_CallFunction>(EventGraph);
	AddToViewportNode->NodePosX = 600;
	AddToViewportNode->NodePosY = 0;
	EventGraph->AddNode(AddToViewportNode, false, false);
	AddToViewportNode->SetFromFunction(AddToViewportFn);
	AddToViewportNode->CreateNewGuid();
	AddToViewportNode->PostPlacedNewNode();
	AddToViewportNode->AllocateDefaultPins();

	// Wire exec chain: BeginPlay -> CreateWidget -> AddToViewport
	BeginPlayNode->FindPinChecked(UEdGraphSchema_K2::PN_Then)->MakeLinkTo(
		CreateWidgetNode->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
	CreateWidgetNode->FindPinChecked(UEdGraphSchema_K2::PN_Then)->MakeLinkTo(
		AddToViewportNode->FindPinChecked(UEdGraphSchema_K2::PN_Execute));

	// Wire data: CreateWidget's ReturnValue -> AddToViewport's target (self)
	CreateWidgetNode->GetReturnValuePin()->MakeLinkTo(
		AddToViewportNode->FindPinChecked(UEdGraphSchema_K2::PN_Self));

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(NewBP);
	FKismetEditorUtilities::CompileBlueprint(NewBP);
	NewBP->MarkPackageDirty();

	return NewBP;
}
