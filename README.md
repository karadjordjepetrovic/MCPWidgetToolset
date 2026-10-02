# UE5 MCP Widget Toolset

A native C++ toolset for Unreal Engine 5 that lets LLM agents (e.g. Claude Code) build and edit
UMG user interfaces through the **Model Context Protocol (MCP)**, using UE's built-in MCP plugin
and Toolset Registry.

## Tools

| Tool | What it does |
|---|---|
| `CreateWidgetBlueprintAsset` | Creates a new Widget Blueprint asset |
| `AddChildWidget` | Adds a child widget to a panel in the widget tree |
| `RemoveWidget` | Removes a widget from the tree |
| `SetWidgetProperty` | Sets a property on a widget |
| `ListWidgetTree` | Returns the widget hierarchy so the agent can inspect current state |
| `BatchSetProperties` | Two-pass transactional batch edit: validates every change first, then applies all of them in one undoable transaction (all-or-nothing) |

## Design

- Implemented as a `UToolsetDefinition`-derived C++ class (`UUMGWidgetToolset`), registered
  through UE's Toolset Registry and exposed over MCP.
- Edits go through editor transactions, so every agent action is undoable.
- `BatchSetProperties` validates first and applies second, so a bad edit never leaves a widget
  half-modified.

## Status

Source code is being migrated to this repository. Planned next step: a **UMG QA/audit agent**
that checks widgets for localization text overflow, gamepad focus-navigation gaps,
design-system conformance and title-safe-zone issues.
