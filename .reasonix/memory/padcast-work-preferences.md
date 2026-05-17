<!-- priority: high -->
<!-- description: PadCast work preferences — simple/readable, match existing style, surface risks, ask before destructive refactors, use cheapest model, idiomatic C++ -->

# PadCast Work Preferences

These preferences must be followed when working on the PadCast project.

- **Prefer simple, readable code over clever optimizations.** The hot path (draw loop) matters, but startup/config code doesn't need micro-optimization.
- **Match existing style** — tabs, brace placement, naming conventions. Consistency across the file matters more than personal preference.
- **Surface risks before implementing.** If a change could break things, is cross-file, or has trade-offs I might not see — flag it first.
- **Ask before destructive refactors.** Renaming symbols, restructuring classes, or touching the build system — check first.
- **Use the cheapest capable model for simple tasks.** Only request the pro model for cross-file refactors, subtle correctness/invariant reasoning, or when the task clearly exceeds what the flash tier can do well.
- **Idiomatic C++ for this project means:** RAII, value semantics where sensible, `const`-correctness, avoiding raw `new`/`delete`, and preferring the standard library over bespoke containers. Don't over-abstract — a straightforward function is better than a template hierarchy with one caller.
