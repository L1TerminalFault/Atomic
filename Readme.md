# atomic UI

A high-performance, single-pass, hardware-accelerated vector graphics and text rendering canvas built from scratch on Vulkan. No massive bloatware dependencies, no electron-style RAM hogging—just pure, raw pixel layout speed running straight on the GPU.

It is currently in its "move fast, break things, and occasionally stare at a blank validation layer screen" early development phase. 

===============================================================================
The Core Blueprint
===============================================================================

Unlike traditional UI toolkits that choke processing cycles by layering separate passes for panels, borders, and text strings, atomic flattens the entire layout into a single, unified instance stream. 

[ Your UI Code ] ---> UIInstance Queue ---> Single Vulkan Draw Call ---> GPU Canvas

Every rectangle, rounded panel, perfect circle, and glyph from the text packing engine maps directly to an irreducible, atomic layout structure. This dispatches to a custom fragment shader that evaluates math strokes, dimensions, and curves instantly.

===============================================================================
Current Arsenal
===============================================================================

* [!] Unified Instance Batching: Draws geometric shapes and raw typography in a single execution pipeline pass.
* [!] Adaptive Vector Primitive Engine: Independent math control over custom radii corners, border stroke positions, and dynamic dotted/dashed outlines.
* [!] Dynamic Type Processing: Built-in FreeType integration caching raw character metrics and packing layouts seamlessly on the fly into a single-channel GPU atlas map.
* [!] Zero-Bleed Memory Guard: Fully isolated data packing to guarantee shape styles never contaminate adjacent canvas resources.

===============================================================================
The Battle Plan
===============================================================================

The foundation is rock-solid, which means the upcoming roadmap expands directly into full vector suite capabilities:

[-] Linear & Radial Gradients: Multi-point color math interpolation mapping directly across primitive coordinates.
[-] Dynamic Image Samplers: Global descriptor array indexing to dynamically map photographs and asset textures into any shape layout.
[-] Paragraph Text Gradients: Spanning fluid color blending smoothly across complete text blocks instead of isolated character boxes.
[-] Arbitrary Polygons: Shifting custom structural nodes out of hardcoded quad generations to support custom charts and vector paths.
[-] SVG Support: Seamless parsing pipelines to translate vector assets straight into live rendering space.

===============================================================================
Contributing & Chaos
===============================================================================

Since the library is in its early stages, breaking changes are the default state of affairs here. If you are comfortable debugging layout matrices, dealing with Vulkan synchronization pipelines, or writing clean graphics abstractions that challenge conventional UI architecture frameworks, feel free to dive in.
