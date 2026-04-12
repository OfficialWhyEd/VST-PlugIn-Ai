# Design System Strategy: Biomechanical Kineticism

## 1. Overview & Creative North Star
The Creative North Star for this design system is **"The Living Engine."** 

We are moving away from the static, "flat" web. This system rejects the notion of UI as a passive surface, instead treating the interface as a dormant biomechanical organism that awakens upon user interaction. We achieve this through **Kinetic Complexity**: the visual language of interlocking plates, shifting geometries, and "pulsing" energy conduits. 

To break the "template" look, we utilize **Intentional Asymmetry**. Containers should not always be perfect rectangles; use clipped corners, staggered heights, and overlapping "armor plates" to create a sense of industrial assembly. The UI should feel as though it was machined, not coded.

---

## 2. Colors & Materiality
The palette is rooted in high-contrast "Power States." Deep, obsidian-toned neutrals provide the "chassis," while vibrant energy colors represent the "ignition."

### The Palette
- **Core Chassis (Background):** `surface` (#121317). A void-like obsidian that allows accents to pop.
- **Primary Energy:** `primary_container` (#DC143C - Cremisi Red). Used for critical actions and core "power sources."
- **Telemetry Data:** `secondary_container` (#FDAF00 - Amber). Reserved for status updates, data visualization, and secondary navigation.
- **The "No-Line" Rule:** We do not use 1px solid borders for sectioning. Boundaries are defined by the physical stacking of plates. Use `surface_container_low` against `surface` to create a "recessed" mechanical bay look.
- **Signature Textures:** Apply a 2% opacity noise or carbon fiber micro-texture to `surface_container_high` elements. This removes the "digital" flatness and introduces a "brushed metal" tactile quality.

---

## 3. Typography: The Monospaced Intel
We use **Space Grotesk** exclusively. Its monospaced rhythm evokes technical readouts and high-end industrial machinery.

- **Display (Large Tech Readouts):** `display-lg` to `display-sm`. Use these for hero numbers or section headers. Letter-spacing should be set to `-0.02em` to feel dense and "engineered."
- **Headlines (Sub-Systems):** `headline-md`. All-caps is preferred here to mimic industrial stamping.
- **Body (Standard Intel):** `body-md`. Use `on_surface_variant` (#E6BDBC) for long-form text to reduce eye strain against the obsidian background.
- **Labels (Telemetry):** `label-sm`. Always paired with `secondary_fixed` (Amber) for a "heads-up display" (HUD) feel.

---

## 4. Elevation & Depth: Tonal Layering
In this design system, "elevation" is not a shadow; it is a **mechanical displacement.**

- **The Layering Principle:** Use the `surface_container` tiers to create a nested hierarchy. 
    - `surface_container_lowest`: Background voids/deep pits.
    - `surface_container`: The main deck.
    - `surface_container_highest`: Active, protruding plates.
- **Ambient Glows:** Instead of grey shadows, use a `primary` (#FFB3B3) glow at 5% opacity for active elements. This mimics light escaping from between mechanical plates.
- **The "Ghost Border" Fallback:** If a tactile edge is required, use `outline_variant` at 15% opacity. This creates a "lathed edge" effect rather than a stroke.
- **Glassmorphism:** Use `surface_bright` with a 40px backdrop blur for floating HUD elements. This simulates a "projection" or a glass sensor array.

---

## 5. Components: The Interlocking Plates

### Buttons (Kinetic Actuators)
- **Primary:** `primary_container` (#DC143C) background. On hover, the button should not just change color; it should "expand" via a 2px outward clip-path animation, revealing a secondary "glow" layer underneath.
- **Tertiary:** No background. Use a "bracket" design (e.g., `[ BUTTON ]`) using the `secondary` token.

### Input Fields (Data Ports)
- **Styling:** Forbid 4-sided borders. Use a single bottom-bar `outline` and a `surface_container_low` background. 
- **Focus State:** The bottom bar transitions from `outline` to `primary` (Cremisi Red), and a subtle "scanline" animation pulses across the field.

### Cards & Lists (System Modules)
- **No Dividers:** Separate list items using `surface_container_low` backgrounds with 4px gaps. This makes the list look like a series of slotted battery cells.
- **Interaction:** On hover, a card should scale by 1.02 and trigger a "mechanical unfolding" (a subtle 3D tilt and an increase in `surface_brightness`).

### Additional Component: "The Energy Conduit"
- A progress bar or separator that uses a gradient from `primary` to `primary_container`. It should feature a "traveling pulse" animation—a high-brightness spark that travels the length of the conduit every 3 seconds.

---

## 6. Do’s and Don’ts

### Do:
- **Use Micro-Interactions:** Every click should feel like a haptic "thud." Use snappy 150ms "Ease-Out-Back" transitions.
- **Embrace Asymmetry:** Stagger the alignment of labels and data points to mimic a dense technical schematic.
- **Layer with Purpose:** Treat the screen as a 3D space where the "core energy" (Red) is always the deepest or most central layer.

### Don’t:
- **No Rounded Corners:** `0px` radius everywhere. The "Living Machine" aesthetic relies on sharp, aggressive, machined edges.
- **No Soft Pastels:** Avoid any colors outside the defined tokens. This system is high-stakes and industrial; soft colors break the immersion.
- **No Standard Transitions:** Avoid simple "fades." If an element appears, it should "slide and lock" or "unfold."

### Accessibility Note:
While the theme is dark, ensure all `label` and `body` text maintains a 4.5:1 contrast ratio against their respective `surface_container` tiers. Use the `amber` telemetry color (#FFB000) for warnings—it provides high visibility against the gunmetal chassis.