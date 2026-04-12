# Design System Strategy: Industrial Precision & Telemetry

## 1. Overview & Creative North Star: "The Tactical Monolith"
The Creative North Star for this design system is **The Tactical Monolith**. We are moving away from the "app" feel and toward a "hardware instrument" reality. Inspired by the brutalist functionality of *Marathon’s* UI and the surgical sophistication of *Ozone 12*, this system treats the screen as a high-density heads-up display (HUD). 

We break the standard grid through **Functional Asymmetry**. Elements are not just placed; they are "docked" into a unified industrial frame. We utilize overlapping telemetry, scanning animations, and high-contrast color shifts to simulate a high-performance AI mastering environment where every pixel represents a live data stream.

## 2. Colors: The High-Contrast Command Center
Our palette is rooted in a deep-space industrial base, punctuated by high-visibility alerts.

*   **Primary (Cremisi Red - #DC143C):** Reserved strictly for critical data—peak clipping, active AI processing states, and high-priority warnings.
*   **Secondary (Amber - #FFB000):** The "Terminal" layer. Used for all telemetry text, readouts, and secondary technical data.
*   **Neutral (Gunmetal - #131313):** The foundation. We use varying depths of grey to define physical geometry.

### The "No-Line" Rule
Prohibit standard 1px solid borders for sectioning. Structural separation is achieved through:
1.  **Background Shifts:** Using `surface_container_lowest` for background "wells" and `surface_container_high` for raised control modules.
2.  **Tonal Transitions:** A subtle transition from `surface` to `surface_container` creates an edge without a stroke.

### Surface Hierarchy & Nesting
Treat the UI as a physical chassis. 
*   **The Chassis (`surface`):** The main background.
*   **The Modules (`surface_container_low`):** Sub-sections nested within the chassis.
*   **The Interactive Nodes (`surface_container_highest`):** Knobs, sliders, and buttons that sit closest to the user.

### The "Glass & Gradient" Rule
Floating overlays (like pop-out analyzers or menu systems) must use **Glassmorphism**. Apply `surface_container` with a 60-80% opacity and a `20px` backdrop-blur. 

### Signature Textures
Apply a subtle `primary_container` to `primary` linear gradient on critical active buttons. To enhance the "Surgical" feel, add a global **CRT Scanline Overlay**—a repeating 2px pattern of 2% opacity black lines—to give the digital interface a hardware-integrated soul.

## 3. Typography: The Telemetry Scale
We utilize a strictly monospaced and technical font family (**Space Grotesk**) to reinforce the feeling of a machine-read interface.

*   **Display (Surgical Readouts):** Used for large DB meters or LUFS values. High impact, always in `secondary` (Amber).
*   **Headline/Title (Modular Headers):** Used for section labels (e.g., "AI COMPRESSOR"). Always uppercase with 0.1em letter spacing.
*   **Body (Operational Text):** Technical descriptions or parameter values.
*   **Label (Telemetry):** The smallest data points. Use `label-sm` for "micro-data" that mimics hardware etching.

## 4. Elevation & Depth
In a tactical interface, depth is simulated through light emission rather than physical shadows.

*   **The Layering Principle:** Stack `surface_container_lowest` (recessed) and `surface_container_high` (protruding) to create a mechanical topography.
*   **The "Internal Glow":** Instead of external shadows, use a `1px` inner-shadow or "glow" on active modules. This should use the `primary` (Cremisi) or `secondary` (Amber) color at 15% opacity to simulate light leaking from behind a physical panel.
*   **The "Ghost Border" Fallback:** If a boundary is required for a floating window, use `outline_variant` at 20% opacity. It must look like a faint reflection on glass, not a solid line.
*   **Interactive Illumination:** When a control is active, its "glow" increases. A knob doesn't just turn; it illuminates the surface around it.

## 5. Components

### Buttons (Tactical Switches)
*   **Primary:** Background `primary_container`, text `on_primary_container`. Features a 1px `surface_tint` top edge to simulate a "beveled" mechanical switch.
*   **Tertiary:** No background. Text in `secondary`. On hover, a `10%` `secondary` background appears with a `backdrop-blur`.

### Knobs & Sliders (Surgical Controls)
*   **Knobs:** Do not use standard circles. Use a segmented ring design. The "active" portion of the ring uses a `primary` glow.
*   **Sliders:** Track is `surface_container_highest`. The thumb is a sharp, square-edged block in `primary`.

### Input Fields (Terminal Inputs)
*   Background is always `surface_container_lowest`. 
*   Text is `secondary` (Amber). 
*   No rounded corners (`0px`).
*   Active state: A 1px `secondary` bottom-border with a faint "flicker" animation to mimic a CRT terminal.

### Cards & Modules
*   Forbid dividers. Use a `16px` vertical gap and a background shift to `surface_container_low`. 
*   Header areas should have a subtle scanline texture distinct from the main body.

### Status LEDs (Bespoke Component)
*   Small circular indicators next to labels. 
*   **Off:** `surface_variant`. 
*   **Active:** `primary` with a 4px blur "bloom" effect to simulate a real light-emitting diode.

## 6. Do's and Don'ts

### Do:
*   **Embrace the Grid:** Everything must feel mathematically aligned. Use 8px increments.
*   **Use Intentional Asymmetry:** Let a meter on the right be taller than the controls on the left to create a "data-heavy" professional look.
*   **High Contrast:** Keep text high-contrast against the gunmetal background to ensure readability in dark studio environments.

### Don't:
*   **No Rounding:** Use `0px` radius for everything. Rounded corners break the industrial hardware aesthetic.
*   **No Soft Gradients:** Avoid "sky" or "nature" gradients. Gradients should only be used to simulate metallic sheen or light emission.
*   **No Standard Iconography:** Avoid "cute" or rounded icons. Use sharp, geometric, or wireframe-style icons.