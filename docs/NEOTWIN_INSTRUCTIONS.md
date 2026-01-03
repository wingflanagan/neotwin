NeoTwin Instructions (Theme + Roadmap)

Overview
- Theme system: optional `twtheme` file for chrome glyphs and default colors.
- `twinrc` strings now accept UTF-8 directly (with CP437 fallback).
- Roadmap: see `docs/NEOTWIN_PLAN.md`.

Theme System Quickstart
1) Copy the sample file to your config directory:
   - `cp twtheme ~/.config/twin/twtheme`
   - or `cp twtheme $XDG_CONFIG_HOME/twin/twtheme`
2) Edit `twtheme` and adjust glyphs/colors.
3) Restart the WM (or restart NeoTwin) to reload the theme.

Where `twtheme` is loaded from (in order)
- `$XDG_CONFIG_HOME/twin/twtheme`
- `$HOME/.config/twin/twtheme`
- `${CONFDIR}/twtheme` (system-wide)
- `./twtheme`

Note: In secure mode, only `${CONFDIR}/twtheme` is read.

Theme File Format (`twtheme`)
- One directive per line.
- `#` starts a comment (unless inside quotes).
- Strings must be quoted with `"..."`.
- Escapes supported: `\xNN`, `\ooo`, `\n`, `\t`, etc.
- Direct UTF-8 is allowed if your editor/font supports it.

Directive: Chrome
- Purpose: override chrome glyphs used for borders, scrollbars, tabs, etc.
- Syntax: `Chrome <Target> "<String>"`
- Targets and required lengths:
  - `GadgetResize` (2 glyphs)
  - `ScrollBarX`   (3 glyphs)
  - `ScrollBarY`   (3 glyphs)
  - `TabX`         (1 glyph)
  - `TabY`         (1 glyph)
  - `ScreenBack`   (2 glyphs)

Examples (CP437 bytes)
- `Chrome ScrollBarX "\xB1\x11\x10"`
- `Chrome TabX "\xDB"`

Directive: DefaultColors
- Purpose: set default chrome colors used when new windows/widgets are created.
- Syntax: `DefaultColors <Target> <ColorSpec>`
- Targets:
  - `Gadgets`, `Arrows`, `Bars`, `Tabs`, `Border`, `Disabled`, `SelectDisabled`
- Color names: `Black`, `Blue`, `Green`, `Cyan`, `Red`, `Magenta`, `Yellow`, `White`
- Optional intensity: `High` or `Bold` (same meaning)
- ColorSpec grammar:
  - `[High] Color [On [High] Color]`
  - `On [High] Color` (foreground defaults to White)

Examples
- `DefaultColors Border High White On Blue`
- `DefaultColors Disabled Black On Black`
- `DefaultColors Gadgets On High Blue`

Notes and Limitations
- Default colors apply to new windows/widgets; existing windows keep their current colors.
- Chrome glyph changes are global and apply on the next redraw.
- If a line is invalid, the parser logs a warning and keeps previous values.

`twinrc` (Border/Button strings)
- Border and button strings are decoded as UTF-8 when possible.
- If UTF-8 is invalid (or contains control bytes), bytes are treated as CP437.
- Use `\xNN` escapes for CP437 box-drawing characters.
- `Border` expects 9 glyphs (3x3); `Button` expects 2 glyphs.

Reloading Theme and Config
- Theme is loaded when the WM reloads `twinrc`.
- Easiest path: restart NeoTwin / ntwin_server.
- Alternative: add a `Restart ""` entry to a menu and invoke it.
- `Refresh` only redraws; it does not reload theme or config.

Roadmap Reference
- See `docs/NEOTWIN_PLAN.md` for the staged plan (theme system, desktop shell, system services).
