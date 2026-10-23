# Derived interface metadata

[简体中文](DERIVED.zh-CN.md)

The Taikou profile contains build identifiers, shader content-hash identifiers, stage/binding/query facts, pairing lists and pipeline metadata needed to identify interfaces. These describe how the compatibility layer connects to user-supplied inputs; they are not executable guest code or complete shader implementations.

- `dm/games/0100346017304000/game.tsv`: title/module identities, verified build IDs, memory budgets, handheld mode and shader profile settings.
- `shader_map.tsv`, `shader_facts.tsv`, `shader_pairs.tsv`, conflicts/bf_want/pso_manifest tables: interface keys, source locations, stage and translation facts.
- `dmsh_chain.h`: a fingerprint of the source translation chain.
- NVN/Horizon headers and generic rewriting rules: API/ABI and host adaptation knowledge inherited from upstream.

The repository contains no original NSO segments, rewritten `.patched.bin`, game-derived `.xgs`/SPIR-V, RomFS, game art/text/movies, saves, keys or firmware. Tools and project-owned control art are distinct from game content and retain their notices. All actual game implementations are generated privately by `scripts/prepare_game.py` from the user's copy.

Documentation exception: docs/screenshots/ contains explicitly labeled emulator screenshots for illustration, not extracted runtime assets; game-image rights are reserved by their owners.
