// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ToolsetRegistry/ToolsetDefinition.h"
#include "Components/Widget.h"
#include "UMGToolSetExt.generated.h"

class UWidgetBlueprint;
class UBlueprint;

/** Common anchor/pivot presets mirroring the UMG Designer's anchor preset grid. */
UENUM(BlueprintType)
enum class EUMGAnchorPreset : uint8
{
	TopLeft,
	TopCenter,
	TopRight,
	CenterLeft,
	Center,
	CenterRight,
	BottomLeft,
	BottomCenter,
	BottomRight,
	/** Anchors span the full parent canvas; Margin is used as a classic four-sided inset and Size is ignored. */
	StretchFill
};

/**
 * Project-local extensions to UMGToolSet.
 *
 * Registered as its own toolset ("UMGToolSetExt.UMGToolSetExt") alongside the
 * engine's UMGToolSet - no engine files are modified.
 */
UCLASS(BlueprintType)
class UUMGToolSetExt : public UToolsetDefinition
{
	GENERATED_BODY()

public:
	/**
	 * Sets a widget's brush to a flat solid color in one call.
	 *
	 * Supports UImage (Brush/ColorAndOpacity) and UBorder (Background/BrushColor).
	 * Assigns the engine white-square texture as the brush resource so the tint
	 * color actually has something to color - a bare tint on a brush with no
	 * resource object renders nothing.
	 *
	 * @param WidgetBlueprint The widget blueprint that owns Widget (used to mark the asset dirty).
	 * @param Widget The widget to recolor. Must be a UImage or UBorder.
	 * @param Color The solid color to apply.
	 * @return true if Widget was a supported type and was updated.
	 */
	UFUNCTION(meta = (AICallable), Category = "UMG|Styling")
	static bool SetSolidColor(UWidgetBlueprint* WidgetBlueprint, UWidget* Widget, FLinearColor Color);

	/**
	 * Positions a widget in its parent Canvas Panel using a named anchor preset, instead of
	 * hand-computing anchors/alignment/offsets. For point presets (everything but StretchFill),
	 * Anchors and Alignment are both set to the preset's corner/edge/center point, Size becomes
	 * the slot size, and Margin insets the widget from whichever edges the preset touches.
	 *
	 * @param WidgetBlueprint The widget blueprint that owns Widget (used to mark the asset dirty).
	 * @param Widget The widget to position. Must be a direct child of a Canvas Panel.
	 * @param Preset Which anchor/pivot preset to apply.
	 * @param Size Widget size in slate units. Ignored for StretchFill.
	 * @param Margin Inset from the edges the preset touches. For StretchFill, applied to all four sides.
	 * @return true if Widget's slot was a Canvas Panel slot and was updated.
	 */
	UFUNCTION(meta = (AICallable), Category = "UMG|Layout")
	static bool SetAnchorPreset(UWidgetBlueprint* WidgetBlueprint, UWidget* Widget, EUMGAnchorPreset Preset, FVector2D Size, FMargin Margin);

	/**
	 * Resizes a widget in its Canvas Panel slot, keeping its current anchors, alignment, and
	 * position untouched - only Offsets.Right/Bottom (the size fields, for point anchors) change.
	 *
	 * @param WidgetBlueprint The widget blueprint that owns Widget (used to mark the asset dirty).
	 * @param Widget The widget to resize. Must be a direct child of a Canvas Panel.
	 * @param NewSize The new width/height in slate units.
	 * @return true if Widget's slot was a Canvas Panel slot and was updated.
	 */
	UFUNCTION(meta = (AICallable), Category = "UMG|Layout")
	static bool ResizeCanvasSlot(UWidgetBlueprint* WidgetBlueprint, UWidget* Widget, FVector2D NewSize);

	/**
	 * Creates a new Actor Blueprint whose BeginPlay creates an instance of the given Widget
	 * Blueprint and adds it to the viewport - the whole "make this widget visible when I press
	 * Play" workflow in one call, instead of hand-wiring CreateWidget -> AddToViewport nodes.
	 *
	 * @param WidgetBlueprintPath Content path of the Widget Blueprint to display (e.g. "/Game/UI/WBP_MyWidget").
	 * @param FolderPath Content folder for the new Actor Blueprint (e.g. "/Game/UI").
	 * @param ActorAssetName Name for the new Actor Blueprint asset.
	 * @return The new, compiled Actor Blueprint, or nullptr on failure. Place an instance of its
	 *         generated class in a level (e.g. via SceneTools.add_to_scene_from_class) to see the
	 *         widget on Play.
	 */
	UFUNCTION(meta = (AICallable), Category = "UMG|Creation")
	static UBlueprint* CreateWidgetDisplayActor(const FString& WidgetBlueprintPath, const FString& FolderPath, const FString& ActorAssetName);
};
