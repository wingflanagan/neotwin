NeoTwin Plan (working draft)

Goals
- Improve look-and-feel without replacing the core server.
- Preserve a coherent, multi-client desktop experience.
- Add a configurable menu and theming that users can change easily.
- Enable system integration through dedicated shell apps and services.

Guiding approach
- Keep the NeoTwin server as the window manager/compositor.
- Add a desktop shell client to provide menus, panels, and settings UX.
- Evolve config and theming in small, backward-compatible steps.

Phased plan
1) Theme system (current focus)
   - Add rc directives for chrome glyphs and default colors.
   - Accept UTF-8 glyphs (with CP437 fallback) to allow modern line art.
   - Provide commented examples in twinrc for borders, buttons, and chrome.
   - Keep existing defaults as a fallback.

2) Desktop shell + menu configuration
   - Create a full-screen shell client for menu, taskbar, and settings.
   - Define a user-editable menu format (categories, actions, keybinds).
   - Allow hot reload of menu config.
   - Add app metadata (name, group, actions) for integration.

3) System integration services
   - Notifications, clipboard, and file picker as core services.
   - Session management (autostart, restore layout).
   - OS configuration entry points via shell apps.

4) Toolkit and widget modernization
   - Expand widget set and input handling ergonomics.
   - Improve focus model and theming hooks for widgets.
   - Stabilize an app-facing UI API for consistent look and behavior.
