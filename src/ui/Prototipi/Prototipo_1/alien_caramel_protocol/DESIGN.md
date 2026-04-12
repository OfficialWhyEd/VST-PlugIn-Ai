# Design System Strategy: The Biological Terminal

## 1. Overview & Creative North Star: "The Living Machine"

This design system is an evolution of rigid terminal interfaces into something more visceral and sentient. The Creative North Star is **"The Living Machine"**—a digital ecosystem where industrial hardware meets biological fluidity. We are moving away from the "static grid" of traditional dashboards toward a UI that feels grown rather than built.

To break the "template" look, we utilize **Intentional Asymmetry**. Layouts should avoid perfect bilateral symmetry; instead, balance a heavy data-dense container on the left with breathable, "liquid" negative space or ambient telemetry on the right. Elements should overlap slightly, using the z-axis to suggest a specimen suspended in fluid rather than a flat document.

## 2. Colors & The "Liquid" Surface

The palette is rooted in the tension between the obsidian depths of space and the warm, viscous glow of amber life-support systems.

### Palette Roles
*   **The Obsidian Void (`surface`, `background` - #0e0e0e):** The foundational substrate. It is never "grey," but a deep, infinite black.
*   **Stable Telemetry (`primary` - #ffd16f, `primary_container` - #ffbf00):** Used for nominal data, active states, and "safe" interactions.
*   **The Cremisi Pulse (`secondary` - #ff6f77, `secondary_container` - #bf0030):** Reserved for high-alert, clipping, or biological anomalies. Use sparingly to maintain its psychological impact.
*   **The Tonal Bridge (`tertiary` - #ffaa78):** Represents the "Liquid Caramel" transition—the intersection of hardware and biology.

### The Rules of Engagement
*   **The "No-Line" Rule:** Standard 1px solid section dividers are strictly prohibited. Boundaries are defined by the transition from `surface_container_low` to `surface_container_high`.
*   **Surface Hierarchy & Nesting:** Treat the UI as a series of biological layers. A `surface_container_lowest` (deepest black) panel might host `surface_container` cards, which in turn hold `surface_bright` interactive elements. This creates a "submerged" depth.
*   **The Glass & Gradient Rule:** High-level floating panels must use **Glassmorphism**. Apply `surface` at 60% opacity with a 20px Backdrop Blur. 
*   **Signature Textures:** Main CTAs must use a linear gradient from `primary` (#ffd16f) to `primary_container` (#ffbf00) at a 135-degree angle to mimic the refraction of light through thick caramel.

## 3. Typography: Monospaced Authority

We use **Space Grotesk** exclusively. Its monospaced DNA provides the "Industrial" tech feel, while its idiosyncratic letterforms provide the "Biological" soul.

*   **Display (3.5rem - 2.25rem):** Use for terminal headers and critical status codes. Letter spacing should be set to -0.02em to feel "pressurized."
*   **Headline (2rem - 1.5rem):** Used for section titles. Always in Uppercase to denote system authority.
*   **Body (1rem - 0.75rem):** All functional data. Use `on_surface_variant` (#adaaaa) for secondary telemetry to ensure the `primary` amber pops.
*   **Label (0.75rem - 0.6875rem):** For micro-data and timestamps. These should often be accompanied by a 50% opacity to mimic "ghosted" CRT remnants.

## 4. Elevation & Depth: Tonal Layering

Traditional shadows are replaced by **Ambient Bioluminescence**.

*   **The Layering Principle:** Instead of shadows, use "Surface Lifting." To make a card feel interactive, move it from `surface_container_low` to `surface_container_highest` on hover.
*   **The "Bioluminescent" Border:** While solid borders are banned for sectioning, interactive "specimen" panels utilize a 1px border using `primary` at 30% opacity. This border must feature a CSS keyframe animation "pulse" (opacity 30% to 60%) to simulate a rhythmic breath.
*   **Ghost Border Fallback:** If accessibility requires a hard container, use `outline_variant` at 15% opacity. It should feel like a faint etched line on a glass screen.
*   **Effects:** Apply a subtle **Chromatic Aberration** (0.5px shift) to the `secondary` (Cremisi) elements when in an error state to simulate system degradation.

## 5. Components

### Buttons
*   **Primary:** Gradient fill (`primary` to `primary_container`). No border. High-contrast `on_primary_fixed` text.
*   **Tertiary/Ghost:** No fill. `primary` text. On hover, a subtle `surface_variant` background "bleeds" in from the center.

### Input Fields
*   **State:** Underline-only (2px). The underline uses the "Bioluminescent" pulse.
*   **Typography:** All inputs use `body-md` Space Grotesk.

### Cards & Lists
*   **Rule:** No dividers. Use a 16px vertical gap (`spacing-md`).
*   **Selection:** A selected list item should change its background to `surface_container_high` and gain a `secondary` (Cremisi) 2px "indicator pip" on the far left.

### Specialized Components: The Telemetry Feed
*   **Scanline Overlay:** A global fixed `div` with a repeating linear gradient (black to transparent) at 2px intervals, set to 3% opacity. 
*   **The Liquid Progress Bar:** Instead of a solid fill, use a "viscous" CSS gradient that moves slowly from left to right, mimicking the flow of fluid through a tube.

## 6. Do's and Don'ts

### Do
*   **Do** use intentional misalignment. A header that is offset by 8px from the grid creates a bespoke, organic feel.
*   **Do** lean into the "Amber on Black" high-contrast ratio for readability.
*   **Do** use `body-sm` for technical "flavor text" that doesn't need immediate reading but adds to the industrial atmosphere.

### Don't
*   **Don't** use standard "Material" blue or "Bootstrap" rounded corners. Use the `sm` (0.125rem) or `none` values for a sharper, more aggressive industrial edge.
*   **Don't** use 100% white (#ffffff) for large blocks of text; it breaks the "submerged" aesthetic. Use `on_surface_variant`.
*   **Don't** animate with "Ease-in-out." Use custom cubic-beziers that feel "heavy" and "viscous" (e.g., `cubic-bezier(0.2, 0.8, 0.2, 1)`).