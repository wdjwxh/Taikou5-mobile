# Bring your own game files

[简体中文](GAME_FILES.zh-CN.md) · [README](../README.md)

Package integrity is checked against the file hashes in that package's own game.json, not a fixed ZIP hash from the developer. ZIP name, timestamps and compression may differ. The shell still requires the documented title/build and package schema; another region or game update needs a compatible profile rather than removing these checks.

## Supported input

This preview accepts one verified build: Taikou Risshiden V DX, Switch title `0100346017304000`, update `v196608`. Cat Officer DLC `0100346017305001` is excluded and not required.

Provide a legally obtained dump from your own game, prepared with your own lawful extraction tools. This repository provides no game downloads, keys, system firmware, or instructions for obtaining someone else's content. The preparation script takes extracted files, **not NSP/NSZ/XCI/IPA directly**.

The input must contain original, unmodified NSO modules and the **complete base+update merged RomFS**:

```text
D:/Games/Taikou5-merged/
  ExeFS/
    main
    subsdk0
    sdk
  RomFS/
    TDATA/BUSTUP.G1T
    ... all other updated-game files ...
```

Use your dump/extraction tool's base+update merge/export function. An update patch alone is not a complete RomFS: BKTR references base data. Copying a few patch files over base data, renaming an IPA's rewritten modules, or mixing updated code with base-only RomFS is not supported. Keep the Cat Officer DLC separate. The tested merged RomFS has 1324 files; this preview checks that count and the marker. That check is not a substitute for correctly merging the dump.

| Module | Expected NSO build ID (first 16 bytes) |
|---|---|
| main | `f90fdcaf95cce279df79279102062c88` |
| subsdk0 | `434729719bfacada3bd0ed4842a10e55` |
| sdk | `dfa12b7019fffbe725fa4b4f1335988c` |

The script checks each original segment's SHA-256 against the NSO header before rewriting. Different versions stop explicitly rather than applying unverified patches.

## Tools

The tested preparation host is Windows. Install Python 3.10+ (3.14 tested), the dependencies below, .NET **10 runtime**, and a recent SPIRV-Tools providing `spirv-opt` and `spirv-val` (must support `--trim-capabilities`). Android SDK/NDK are not needed to prepare resources for an existing shell APK.

```powershell
python -m pip install -r requirements.txt
```

The pinned translator DLLs are generic open-source tools, vendored from nxnative with sources and notices. Do not rebuild them casually or replace them with another Ryujinx build: the shader interfaces depend on the pinned toolchain. SPIR-V is validated against Vulkan 1.1. Translation on the PC produces portable `.xgs` bundles; device Vulkan pipeline creation is a separate step performed by the runtime. This is not precompiling for one phone GPU.

## Generate a personal import ZIP

Run at the repository root. Replace paths with your own. Use a **new work directory and output filename**, both outside the repository; the script refuses to overwrite them. Prefer ASCII work paths on Windows because some native SPIR-V tools fail on Unicode paths.

```powershell
python scripts/prepare_game.py --exefs "D:/Games/Taikou5-merged/ExeFS" --romfs "D:/Games/Taikou5-merged/RomFS" --work "D:/Taikou5-private/work-01" --output "D:/Taikou5-private/Taikou5-game.zip" --dotnet "C:/Program Files/dotnet/dotnet.exe" --spirv-opt "D:/Tools/SPIRV-Tools/bin/spirv-opt.exe" --spirv-val "D:/Tools/SPIRV-Tools/bin/spirv-val.exe"
```

With tools on PATH, omit their three options. The pipeline rewrites `main/subsdk0/sdk` for Android, scans RomFS/main, verifies the expected shader key set, translates and validates all 212 shaders, then creates a ZIP64 package containing `game.json`, `out/`, `RomFS/`, and `shaders/`. The manifest records file sizes, SHA-256 digests, title and module builds. Saves are never packaged.

Allow space for original inputs, scratch output and a roughly 3.3 GiB transfer ZIP. Keep the private work folder for diagnostics; a failure does not delete your original inputs. Its output is **game content**, even though the source project is open source. Do not commit or upload it.

Next: [install and import](ANDROID.md). For a friend, share the public shell and these instructions; the friend supplies and prepares their own game. No public release includes a ready-to-play game ZIP.
