# Build the shell APK

[简体中文](BUILD.zh-CN.md) · [README](../README.md)

No game files are required for this build. The repository vendors the needed runtime, Vulkan backend, decoder and shader translator sources/tools; no Git submodules are required. Retained Apple/reference sources are inherited infrastructure and do not imply an iOS build of this project.

## Tested Windows toolchain

- Python 3.10+; CMake 3.22+ and Ninja on PATH.
- Android NDK r28b; JDK 17.
- Android SDK build tools 34-compatible (`aapt2`, `d8`, `zipalign`, `apksigner`), and platform 34 `android.jar`.
- Install from the official [Android SDK/NDK tools](https://developer.android.com/studio), [CMake](https://cmake.org/download/), [Python](https://www.python.org/downloads/), and a JDK 17 distribution. Point each option at the unpacked directory, not a ZIP.

Run at the repository root; all paths are examples:

```powershell
python scripts/build_apk.py --ndk "D:/Tools/android-ndk-r28b" --jdk "D:/Tools/jdk-17" --build-tools "D:/Tools/Android/sdk/build-tools/34.0.0" --android-jar "D:/Tools/Android/sdk/platforms/android-34/android.jar" --work "D:/Taikou5-build"
```

Output: `D:/Taikou5-build/apk/Taikou.apk`. The script generates the game-profile header from interface metadata, configures CMake/Ninja for `arm64-v8a` and API 31, builds the runtime, compiles the Java shell, packages control assets, aligns/signs the APK and verifies its signature. The process stops on failed native builds; it does not silently package an old `.so`.

The current preview uses the development shell's target SDK 28 and `debuggable=true`. It is a sideloading preview, not a Play Store production build. Do not claim current target-SDK compliance. Release hardening, store distribution, lifecycle recovery and a stable signing setup remain future work.

## Signing and updating

The packaging script creates a **local development key** at `work/apk/debug.keystore` on first use. Keep it outside Git and back it up privately; subsequent builds reuse it. APK updates require the same signing key. A separately built shell with a different key cannot overwrite the prepared APK; uninstalling to change keys also removes app-private files and saves. Do not uninstall to resolve a signature mismatch without first backing up your data. The prepared APK's private key is never distributed.

The build is scriptable, but byte-for-byte reproducibility is not claimed (ZIP timestamps, compiler paths, tool versions and signing may differ). Check hashes and version metadata when publishing. [Upstream provenance](UPSTREAM.md).

## Separate game preparation

[prepare_game.py](../scripts/prepare_game.py) additionally needs `requirements.txt`, .NET 10 and SPIRV-Tools. It runs separately using personal inputs outside the source tree. See [GAME_FILES](GAME_FILES.md). The APK build never calls game preparation.
