# Design System Document: Industrial Terminal Interface

## 1. Overview & Creative North Star

### Creative North Star: "The Analog Supercomputer"
This design system eschews the soft, rounded friendliness of modern web design in favor of a rigid, high-fidelity industrial aesthetic. Inspired by the 1994 "Marathon" terminal interfaces, the goal is to create a UI that feels like an active piece of hardware—a functional piece of machinery salvaged from a deep-space research vessel.

To break the "template" look, we utilize **Intentional Asymmetry** and **Information Density**. Layouts should feel "data-heavy," with asymmetrical panels and metadata clusters that suggest a complex underlying system. We move beyond standard grids by using overlapping scanline textures and "electrical" glows that bleed across boundaries, ensuring the interface feels alive and energized.

---

## 2. Colors

The palette is rooted in high-contrast "Terminal" logic. We use deep, light-absorbing foundations to make our neon accents feel emissive.

### Surface Hierarchy & Nesting
Depth is achieved through **Tonal Layering** rather than traditional elevation.
*   **Base:** `surface` (#131314) serves as the primary chassis.
*   **Recessed Areas:** Use `surface_container_lowest` (#0e0e0f) for background data feeds or inactive terminal banks.
*   **Raised Modules:** Use `surface_container_high` (#2a2a2b) for active control panels.
*   **The "No-Line" Rule:** Prohibit 1px solid solid borders for sectioning. Boundaries are defined by the shift from `surface` to `surface_container_low`. 

### The "Glass & Gradient" Rule
Floating HUD elements must utilize `surface_variant` with a 60-80% opacity and a `backdrop-filter: blur(12px)`. This creates the "semi-transparent floating panel" effect required for the high-tech atmosphere. Apply a subtle linear gradient to main CTAs (from `primary` to `primary_container`) to simulate a glowing phosphor screen.

---

## 3. Typography

**Font Family:** Space Grotesk (Interpretive Monospaced Feel)
*The system uses Space Grotesk to bridge the gap between strict monospaced utility and high-end editorial readability.*

*   **Display (L/M/S):** Used for large signal readouts or patch names. Apply a 0.5px `text-shadow` in `primary` to simulate tube-glow and a subtle 1px horizontal chromatic aberration (red/cyan shift) to mimic CRT misalignment.
*   **Headline & Title:** Used for module headers (e.g., "OSCILLATOR BANK A"). Always uppercase with 0.1em letter spacing.
*   **Body & Label:** Strict data presentation. Use `label-sm` for technical metadata (e.g., "SAMP_RATE: 44.1k").

Typography conveys the brand identity by treating text as **Data First, Aesthetic Second**. Every piece of text should look like it is being "rendered" by the system in real-time.

---

## 4. Elevation & Depth

### The Layering Principle
Forget shadows; we use **Light Emission**. Since this is a terminal, objects don't "cast shadows" on a dark screen—they "emit light." 
*   Instead of a drop shadow, use a `box-shadow` with the `surface_tint` color at 10% opacity and a 20px blur to create an ambient "glow" around active modules.

### The "Ghost Border" Fallback
While 1px solid borders are forbidden for sectioning, we utilize **Glowing Ghost Borders** for active states. Use `outline_variant` at 20% opacity. For a "Focused" state, use `primary` at 100% with a 2px outer blur to simulate an electrical short-circuit or high-voltage connection.

---

## 5. Components

### Buttons
*   **Primary (Action):** Rectangular (`0px` radius). Background: `primary_container`. Text: `on_primary_container`. High-intensity glow on hover.
*   **Secondary (Control):** `outline` variant. No background. 1px ghost border. 
*   **States:** On `Active`, the background should flicker slightly (opacity 90% to 100%) to simulate a hardware terminal.

### Input Fields / Dials
*   **Text Inputs:** Use `surface_container_highest` with a blinking underscore cursor in `secondary`. 
*   **Dials (VST Specific):** Represented as circular data-rings using `secondary` (Terminal Green). No skeuomorphic plastic; use "digital twin" rings that fill as the value increases.

### Cards & Lists
*   **The Divider Rule:** Forbid 1px divider lines. Separate list items using 8px of vertical whitespace and a background shift to `surface_container_low` for every second item (zebra striping).

### Signature Component: "The Signal Monitor"
A custom component featuring a real-time waveform or spectrogram. Background: `surface_container_lowest`. Overlay: `CRT Scanline Pattern` (2px black lines at 10% opacity). All data rendered in `secondary_fixed`.

---

## 6. Do's and Don'ts

### Do:
*   **DO** use intentional asymmetry. A panel on the left may be 300px while the right is 342px to suggest specialized hardware.
*   **DO** use "Electrical" motion. Transitions should be instant or involve a "scan-in" effect rather than soft fades.
*   **DO** ensure all text remains legible behind the chromatic aberration. Keep the "offset" to less than 1.5px.

### Don't:
*   **DON'T** use rounded corners. Everything is `0px`. Roundness suggests consumer-grade comfort; this system is for professional industrial utility.
*   **DON'T** use standard grey shadows. Use `primary` or `secondary` tints for ambient glows.
*   **DON'T** use "humanist" or serif fonts. They break the terminal illusion.

### Accessibility Note:
Despite the "Data-Heavy" feel, ensure that `on_surface` text against `surface` maintains a contrast ratio of at least 7:1. The "Terminal Green" (`secondary`) should be reserved for successful states or active signals, while "Muted Red" (`on_tertiary_container`) is strictly for critical system warnings or clipping.