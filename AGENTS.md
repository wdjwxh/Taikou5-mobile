# Working on Taikou5 Mobile

- This is the Android-only Taikou Risshiden V DX compatibility project. Preserve nxnative's generic Horizon/NVN design and the official d2r-mobile Vulkan replay; do not replace them with a parallel renderer.
- Scope is title 0100346017304000 with update v196608. Cat Officer DLC is excluded. Fixed module addresses must be gated by verified NSO build IDs.
- Game dumps, decrypted files, rewritten guest programs, translated game shaders, generated personal ZIPs, keys, saves and signing keys stay outside this source tree. Do not add them to Git, CI artifacts or public releases.
- The public APK is a shell only. Project-owned control art and generic shader translator tools may be included with their original license notices.
- English is the default for source, logs and documentation. Maintain README.zh-CN.md and the paired Chinese user guides alongside their English versions.
- Scripts users run belong in scripts/. Avoid machine-specific paths and fail on unsupported inputs. Keep game preparation separate from shell builds.
- Validate consequential changes with the actual emulator/app flow. Do not claim real-phone performance, gesture zoom, full controller support or a full playthrough without evidence.
- No publishing, release upload, commits or agents unless requested. Keep the previous experimental project and its game data/saves intact.
