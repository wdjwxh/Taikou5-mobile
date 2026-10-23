# Contributing

Start with [README](README.md), [build instructions](docs/BUILD.md), and [upstream provenance](docs/UPSTREAM.md). Keep generic Horizon/NVN semantics and the official Vulkan backend; prefer a general fix backed by a failing scene over another rendering path. Gate fixed addresses by the verified NSO build ID.

Use English in source/logs and update both English/Chinese user docs when behavior changes. Include the triggering action, environment, before/after result and meaningful validation. Do not claim device coverage without testing it. Keep unrelated refactors separate and preserve all third-party notices.

Never attach game dumps, keys, original/rewritten modules, translated game shaders, saves, or personal game ZIPs. Redact personal paths from logs. Do not request those files from contributors. Public build artifacts must pass `python scripts/audit_release.py --apk PATH`.
