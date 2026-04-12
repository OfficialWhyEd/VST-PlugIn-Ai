```markdown
# Design System Strategy: The Kinetic Terminal

## 1. Overview & Creative North Star
**The Creative North Star: "Tactile Telemetry"**

This design system moves beyond the static nature of traditional VSTs to embrace an **Ultra-Premium Industrial Terminal** aesthetic. It is a digital cockpit that balances the raw, militaristic utilitarianism of 90s sci-fi (Marathon) with the surgical, high-fidelity precision of modern audio mastering suites (Ozone 12). 

To break the "template" look, we reject fluid, rounded containers in favor of a **Zero-Radius Brutalism**. We utilize intentional asymmetry—where data-heavy telemetry columns offset minimalist control surfaces—creating a rhythmic visual flow that mirrors the complexity of sound. We aren't just building a tool; we are building a high-stakes command center where every pixel is aligned to a rigid grid, yet every interaction feels alive through "breathing" glass and glowing apertures.

---

## 2. Colors & Surface Logic

### The "No-Line" Rule
Traditional dividers are strictly prohibited. Sectioning is achieved through **Tonal Shifting**. A `surface-container-low` panel sitting on a `surface` background creates a clear, architectural boundary without the clutter of 1px solid strokes. The only exception is the "Ghost Border" (see Section 4).

### Color Palette Reference
- **Base (Deep Gunmetal):** `surface` (#131313) – The void in which the interface lives.
- **Accents (Cremisi Red):** `primary-container` (#DC143C) – Reserved strictly for critical data: peak clipping, AI engine engagement, and master "On" states.
- **Telemetry (Amber):** `secondary-container` (#FDAF00) – Used for the "Terminal Log" and real-time numerical readouts. It represents the "intelligence" of the AI.

### Surface Hierarchy & Nesting
Treat the UI as a series of physical plates. 
- **The Chassis:** `surface` (#131313)
- **The Control Trays:** `surface-container-low` (#1C1B1B)
- **The Active Module:** `surface-container-high` (#2A2A2A)
- **The Glass Interface:** Use `surface_variant` at 60% opacity with a 12px `backdrop-blur` to create "frosted" overlays for modal settings or floating menus.

---

## 3. Typography: Precision vs. Clarity

The system utilizes a dual-typeface strategy to distinguish between "Machine Intelligence" and "Human Interaction."

*   **Headlines & Labels (Space Grotesk):** This is your structural voice. It should be tracked out (`letter-spacing: 0.05em`) in all-caps for `label-sm` to mimic industrial stamping.
*   **Data & Telemetry (JetBrains Mono / Space Grotesk):** All numerical values, decibel readings, and terminal logs must use the monospaced character of the system. This ensures that fluctuating numbers don't cause "jitter" in the layout.
*   **Body Copy (Manrope):** Used only for long-form tooltips or help text to ensure maximum legibility against the dark, high-contrast background.

**Hierarchy Note:** Use `display-lg` for the current Gain Reduction or Master Output level, making the most important data point the primary visual anchor of the asymmetrical layout.

---

## 4. Elevation & Depth

### The Layering Principle
We do not use drop shadows to create depth; we use **Light Emission**. Because this is a "Terminal," light comes from the elements themselves (the LEDs and phosphors), not an external sun.

- **Tonal Layering:** Objects are "raised" by shifting from `surface-container-lowest` to `surface-container-highest`.
- **The Ghost Border:** For floating modules, use a 1px border of `outline-variant` (#5C3F3F) at **15% opacity**. This creates a "glass edge" that catches the light without creating a hard box.
- **Glowing Borders:** For active "Cremisi" states, apply a `primary` (#FFB3B3) 1px border with a 4px outer glow (bloom) to simulate a high-energy LED.
- **CRT Scanlines:** Apply a subtle, fixed-position SVG overlay of horizontal lines (2px apart, 3% opacity) across the entire UI to provide that "industrial monitor" texture.

---

## 5. Components

### Buttons & Toggles
- **Primary (Action):** Rectangular. Background: `primary-container`. Text: `on_primary_container`. No rounded corners. On hover, increase brightness; on click, add a 1px `primary` glow.
- **Tactile Toggles:** Use a "Switch" metaphor that shifts background color from `surface-container-highest` (Off) to `primary-container` (On/Active).

### Input Fields & Knobs
- **Precision Sliders:** Eschew the "round knob." Use horizontal or vertical "Meter Sliders" that utilize the `secondary-container` (Amber) for the fill. 
- **Text Inputs:** Use `surface-container-lowest` with a "Ghost Border." The cursor should be a solid Amber block that blinks (Terminal style).

### Cards & Modules
- **Modular Grid:** All cards must align to an 8px grid. 
- **No Dividers:** Use vertical white space or a shift to `surface-bright` to separate the EQ section from the Dynamics section.

### Data Visualizers (Waveforms/Spectrograms)
- **The "Ozone" Aesthetic:** Waveforms should be rendered in `secondary` (Amber) with a gradient fade into `surface`. Peak highlights must flash in `primary-container` (Cremisi Red).

---

## 6. Do's and Don'ts

### Do:
- **Do** use `0px` border-radius for everything. The aesthetic is "Industrial," not "Consumer."
- **Do** use `backdrop-blur` for any element that sits "above" the main telemetry stream.
- **Do** treat the `WhyCremisi` logo as a stamp of quality—place it in a corner with significant negative space.
- **Do** align all text to the "pixel grid" to maintain the crispness of the monospaced fonts.

### Don't:
- **Don't** use standard "Drop Shadows." Use tonal shifts or subtle outer glows (blooms) instead.
- **Don't** use 100% opaque borders. They break the "frosted glass" immersion.
- **Don't** introduce any color outside of the Gunmetal, Cremisi Red, and Amber palette. Any extra color dilutes the "Terminal" authority.
- **Don't** use "Soft" icons. All iconography should be geometric, 1px stroke weight, and sharp-edged.

---

## 7. Signature UI Element: The Terminal Log
Every instance of the VST should feature a "System Telemetry" feed in a corner. Using `label-sm` in Amber, this feed should scroll real-time AI "thoughts" (e.g., `> ANALYZING_LUFS... OK`, `> REDUCING_MIDS_200HZ... DONE`). This reinforces the "AI Master" brand and provides the "soul" of the industrial machine.```